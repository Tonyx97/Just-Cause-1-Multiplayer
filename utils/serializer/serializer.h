#pragma once

// declarations

struct serialization_ctx
{
	std::vector<uint8_t> data;

	uint8_t* begin = nullptr,
		   * end = nullptr;

	int32_t write_offset = -1;

	serialization_ctx() {}

	inline void set_data(uint8_t* src, size_t size)
	{
		data.resize(size);

		memcpy(data.data(), src, size);

		recalculate_begin_end();
	}

	inline void recalculate_begin_end()
	{
		begin = data.data();
		end = begin + data.size();
	}

	inline void append(uint8_t* src, size_t size)
	{
		data.insert(data.end(), src, src + size);
	}

	inline void resize(size_t v)
	{
		clear();
		data.resize(v);
	}

	template <typename T>
	inline T write(const T& value)
	{
		const auto value_ptr = std::bit_cast<uint8_t*>(&value);

		data.insert(write_offset == -1 ? data.end() : data.begin() + write_offset, value_ptr, value_ptr + sizeof(T));

		if (write_offset >= 0)
			write_offset += sizeof(T);

		return value;
	}

	template <typename T>
	inline void write(T* ptr, size_t size)
	{
		const auto casted_ptr = std::bit_cast<uint8_t*>(ptr);

		data.insert(write_offset == -1 ? data.end() : data.begin() + write_offset, casted_ptr, casted_ptr + size);

		if (write_offset >= 0)
			write_offset += size;
	}

	template <typename T>
	inline T read()
	{
		return *std::bit_cast<T*>(std::exchange(begin, begin + sizeof(T)));
	}

	inline void read(void* out, size_t size)
	{
		memcpy(out, std::exchange(begin, begin + size), size);
	}

	inline bool empty() const
	{
		return (end - begin) <= 0;
	}

	inline void clear()
	{
		data.clear();
		begin = end = nullptr;
		write_offset = -1;
	}

	void to_file(const std::string& filename);

	inline void copy_to(uint8_t* dst)
	{
		memcpy(dst, data.data(), data.size());
	}
};

#if defined(JC_CLIENT) || defined(JC_SERVER)
#ifdef JC_DBG
void register_serialized_object(int t, const std::string& name);

#define ADD_SERIALIZED_OBJECT(T, t)	register_serialized_object(t, typeid(T).name())
								 
#else
#define ADD_SERIALIZED_OBJECT(T, t)
#endif
#else
#define ADD_SERIALIZED_OBJECT(T, t)
#endif

template <typename T, typename... A>
struct member_counter;

struct any_type
{
	template <typename T>
	operator T() {}
};

template <typename T, typename... A>
struct member_counter
{
	constexpr static size_t f(int32_t*)
	{
		return sizeof...(A) - 1;
	}

	template <typename U = T, typename enabld = decltype(U { A {}... }) >
	constexpr static size_t f(std::nullptr_t)
	{
		return member_counter<T, A..., any_type>::value;
	}

	constexpr static auto value = f(nullptr);
};

template <typename, template <typename...> typename>
struct is_specialization : std::false_type {};

template <template <typename...> typename container, typename... A>
struct is_specialization<container<A...>, container> : std::true_type {};

template <typename T>
constexpr bool is_plain_copyable_v = std::is_standard_layout_v<T> && std::is_trivial_v<T> && !std::is_pointer_v<T>;

template <typename T>
constexpr bool is_vector_v = is_specialization<T, std::vector>::value;

template <typename T>
constexpr bool is_string_v = std::is_same_v<T, std::string>;

template <typename T>
constexpr bool is_complex_v = !is_plain_copyable_v<T> && !is_string_v<T> && !std::is_pointer_v<T> && !is_vector_v<T>;

template <typename T, typename... A>
void _serialize(serialization_ctx&, const T&, A&&...) requires(is_plain_copyable_v<T>);

template <typename T, typename... A>
void _serialize(serialization_ctx&, const T&, A&&...) requires(is_vector_v<T>);

template <typename T, typename... A>
void _serialize(serialization_ctx&, const T&, A&&...) requires(is_string_v<T>);

template <typename T, typename... A>
void _serialize(serialization_ctx&, const T&, A&&...) requires(is_complex_v<T>);

template <typename T>
T _deserialize(serialization_ctx&) requires(is_plain_copyable_v<T>);

template <typename T>
T _deserialize(serialization_ctx&) requires(is_vector_v<T>);

template <typename T>
T _deserialize(serialization_ctx&) requires(is_string_v<T>);

template <typename T>
T _deserialize(serialization_ctx&) requires(is_complex_v<T>);

// definitions

struct serialize_fn
{
	template <typename T>
	static inline void _do(serialization_ctx& ctx, T& v) { _serialize(ctx, v); }
};

struct deserialize_fn
{
	template <typename T>
	static inline void _do(serialization_ctx& ctx, T& v) { v = _deserialize<T>(ctx); }
};

template <typename T, typename Fn, bool is_const>
void iterate_members(serialization_ctx& ctx, std::conditional_t<is_const, const T&, T&> v)
{
	auto iterate_member = [&]<typename VT, typename... A>(const auto& iterate_member, VT& value, A&&... args) constexpr
	{
		if constexpr (std::is_class_v<VT> && std::is_aggregate_v<VT>)
			iterate_members<VT, Fn, is_const>(ctx, value);
		else Fn::_do(ctx, value);

		if constexpr (sizeof...(args) > 0)
			iterate_member(iterate_member, args...);
	};

	static constexpr auto MAX_AGGREGATES = 10;

	constexpr auto MEMBERS_COUNT = member_counter<T>::value;

	static_assert(MEMBERS_COUNT <= MAX_AGGREGATES, "Cannot use serialization in types with many aggregates");

#define check_and_iterate(count, ...)	if constexpr (MEMBERS_COUNT == count) \
										{ \
											auto& [__VA_ARGS__] = v; \
											iterate_member(iterate_member, __VA_ARGS__); \
										}

	check_and_iterate(1, v0);
	check_and_iterate(2, v0, v1);
	check_and_iterate(3, v0, v1, v2);
	check_and_iterate(4, v0, v1, v2, v3);
	check_and_iterate(5, v0, v1, v2, v3, v4);
	check_and_iterate(6, v0, v1, v2, v3, v4, v5);
	check_and_iterate(7, v0, v1, v2, v3, v4, v5, v6);
	check_and_iterate(8, v0, v1, v2, v3, v4, v5, v6, v7);
	check_and_iterate(9, v0, v1, v2, v3, v4, v5, v6, v7, v8);
	check_and_iterate(10, v0, v1, v2, v3, v4, v5, v6, v7, v8, v9);

#undef check_and_iterate
};

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A&&... args) requires(is_plain_copyable_v<T>)
{
	ADD_SERIALIZED_OBJECT(v, 0);

	ctx.write(v);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A&&... args) requires(is_vector_v<T>)
{
	ADD_SERIALIZED_OBJECT(v, 1);

	ctx.write(v.size());

	for (const auto& e : v)
		_serialize(ctx, e);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A&&... args) requires(is_string_v<T>)
{
	ADD_SERIALIZED_OBJECT(v, 2);

	const auto size = ctx.write(v.size());

	if (size > 0u)
		ctx.write(v.data(), size);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A&&... args) requires(is_complex_v<T>)
{
	ADD_SERIALIZED_OBJECT(v, 3);

	iterate_members<T, serialize_fn, true>(ctx, v);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T>
T _deserialize(serialization_ctx& ctx) requires(is_plain_copyable_v<T>)
{
	return ctx.read<T>();
}

template <typename T>
T _deserialize(serialization_ctx& ctx) requires(is_vector_v<T>)
{
	const auto size = ctx.read<size_t>();

	T v;

	for (size_t i = 0u; i < size; ++i)
		v.push_back(_deserialize<typename T::value_type>(ctx));

	return v;
}

template <typename T>
T _deserialize(serialization_ctx& ctx) requires(is_string_v<T>)
{
	const auto size = ctx.read<size_t>();

	std::string v;

	v.resize(size);

	ctx.read(v.data(), size);

	return v;
}

template <typename T>
T _deserialize(serialization_ctx& ctx) requires(is_complex_v<T>)
{
	T v;

	iterate_members<T, deserialize_fn, false>(ctx, v);

	return v;
}

template <typename... A>
void serialize(serialization_ctx& ctx, A&&... args)
{
	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T>
T deserialize(serialization_ctx& ctx)
{
	return _deserialize<T>(ctx);
}