#pragma once

#define ENET_FEATURE_ADDRESS_MAPPING

#include <enet.h>

#include <shared_mp/objs/all.h>

// declarations

struct serialization_ctx
{
	std::vector<uint8_t> data;
	
	uint8_t* initial_begin = nullptr,
		   * begin = nullptr,
		   * end = nullptr;

	int32_t write_offset = -1;

	inline void add_data(uint8_t* src, size_t size)
	{
		data.resize(size);

		memcpy(data.data(), src, size);
	}

	template <typename T>
	inline T write(const T& value)
	{
		const auto value_ptr = std::bit_cast<uint8_t*>(&value);

		data.insert(write_offset == -1 ? data.end() : data.begin() + write_offset, value_ptr, value_ptr + sizeof(T));

		write_offset += sizeof(T);

		return value;
	}

	template <typename T>
	inline void write(T* ptr, size_t size)
	{
		const auto casted_ptr = std::bit_cast<uint8_t*>(ptr);

		data.insert(write_offset == -1 ? data.end() : data.begin() + write_offset, casted_ptr, casted_ptr + size);

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

	inline int32_t get_total_read() const { return begin - initial_begin; }
};

template <typename, template <typename...> class>
struct is_specialization : std::false_type {};

template <template <typename...> class container, typename... A>
struct is_specialization<container<A...>, container> : std::true_type {};

template <typename T>
constexpr bool is_plain_copyable_v = std::is_trivial_v<T> && !std::is_pointer_v<T>;

template <typename T>
constexpr bool is_vector_v = is_specialization<T, std::vector>::value;

template <typename T>
constexpr bool is_string_v = std::is_same_v<T, std::string>;

template <typename T>
constexpr bool is_net_object_v = std::derived_from<std::remove_pointer_t<std::remove_reference_t<T>>, NetObject>;

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A... args);

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A... args) requires(is_plain_copyable_v<T>);

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A... args) requires(is_vector_v<T>);

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A... args) requires(is_string_v<T>);

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A... args) requires(is_net_object_v<T>);

NetObject* get_net_object_from_lists(NID nid, NetObjectType type);

template <typename T>
T _deserialize(serialization_ctx&) requires(is_plain_copyable_v<T>);

template <typename T>
T _deserialize(serialization_ctx&) requires(is_vector_v<T>);

template <typename T>
T _deserialize(serialization_ctx&) requires(is_string_v<T>);

template <typename T>
T _deserialize(serialization_ctx&) requires(is_net_object_v<T>);

// definitions

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A... args)
{
	ctx.write(v);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A... args) requires(is_plain_copyable_v<T>)
{
	ctx.write(v);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A... args) requires(is_vector_v<T>)
{
	ctx.write(v.size());

	for (const auto& e : v)
		_serialize(ctx, e);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A... args) requires(is_string_v<T>)
{
	const auto size = ctx.write(v.size());

	ctx.write(v.data(), size);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T, typename... A>
inline void _serialize(serialization_ctx& ctx, const T& v, A... args) requires(is_net_object_v<T>)
{
	const auto nid = v ? v->get_nid() : INVALID_NID;

	if (nid != INVALID_NID)
	{
		_serialize(ctx, nid);
		_serialize(ctx, v->get_type());
	}
	else _serialize(ctx, INVALID_NID);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T>
T _deserialize(serialization_ctx& ctx)
{
	return ctx.read<T>();
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
T _deserialize(serialization_ctx& ctx) requires(is_net_object_v<T>)
{
	const auto nid = _deserialize<NID>(ctx);

	if (nid != INVALID_NID)
	{
		const auto type = _deserialize<NetObjectType>(ctx);

		return get_net_object_from_lists(nid, type);
	}

	return nullptr;
}

template <typename... A>
void serialize(serialization_ctx& ctx, A... args)
{
	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T>
std::pair<T, int> deserialize(serialization_ctx& ctx, int offset)
{
	const auto begin = std::bit_cast<uint8_t*>(ctx.data.data() + offset);
	const auto end = std::bit_cast<uint8_t*>(begin + ctx.data.size());

	if (begin >= end)
		return { {}, -1 };

	ctx.initial_begin = begin;
	ctx.begin = begin;
	ctx.end = end;

	auto v = _deserialize<T>(ctx);

	return { v, ctx.get_total_read() };
}

/*#define DESERIALIZE_NID_AND_TYPE(p)			const auto nid = deserialize(p); \
											const auto type = deserialize<NetObjectType>(p)*/