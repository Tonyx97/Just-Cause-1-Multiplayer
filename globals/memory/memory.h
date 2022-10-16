#pragma once

#define ARRAY_SIZE(x) sizeof(x) / sizeof(*x)

#define PRAGMA(x)				_Pragma(#x)
#define ALIGN_PUSH(x)			PRAGMA(pack(push, x))
#define ALIGN_POP()				PRAGMA(pack(pop))

namespace jc
{
#ifdef JC_CLIENT
	template <typename X>
	inline bool protect(uint32_t new_prot, size_t size, X x, ptr offset = 0u)
	{
		DWORD old;
		return !!VirtualProtect(BITCAST(void*, x), size, new_prot, &old);
	}

	inline bool protect_v(uint32_t new_prot, const patch_value& v, ptr offset = 0u)
	{
		DWORD old;
		return !!VirtualProtect(BITCAST(void*, v.address), v.size, new_prot, &old);
	}

	template <typename T, typename X>
	inline bool protect_ex(uint32_t& prot, size_t size, X x, ptr offset = 0u)
	{
		return !!VirtualProtect(BITCAST(void*, x), size, prot, &prot);
	}

	template <typename T, typename X>
	inline T read(X x, ptr offset = 0u)
	{
		if (!x)
			return {};

		return *(T*)(ptr(x) + offset);
	}

	template <typename T, typename X>
	inline void write(T value, X x, ptr offset = 0u)
	{
		if (!x)
			return;

		*(T*)(ptr(x) + offset) = value;
	}

	inline void nop(ptr address, size_t size)
	{
		memset(BITCAST(void*, address), 0x90, size);
	}

	struct Address
	{
		union
		{
			struct
			{
				uint8_t b0 : 8;
				uint8_t b1 : 8;
				uint8_t b2 : 8;
				uint8_t b3 : 8;
			};

			ptr value;
		};

		Address(ptr value) : value(value) {}
	};

	template <typename Tx, typename Ty>
	inline Address calc_call_offset(Tx from, Ty to)
	{
		return Address(static_cast<ptr>(BITCAST(int, to) - BITCAST(int, from)) - 5u);
	}

	template <typename T, typename X>
	inline void write_protect(T value, X x, ptr offset = 0u)
	{
		if (!x)
			return;

		DWORD old;

		const auto address = ptr(x) + offset;

		VirtualProtect(BITCAST(void*, address), sizeof(T), PAGE_EXECUTE_READWRITE, &old);
		InterlockedExchange(BITCAST(volatile unsigned*, address), (unsigned)value);
		VirtualProtect(BITCAST(void*, address), sizeof(T), old, &old);
	}

	template <typename Ret = ptr, typename... A, typename X>
	__declspec(noinline) inline Ret call(X fn, A... args)
	{
		return BITCAST(Ret(A...), fn)(args...);
	}

	template <typename Ret = ptr, typename... A, typename X>
	__declspec(noinline) inline Ret std_call(X fn, A... args)
	{
		return BITCAST(Ret(__stdcall*)(A...), fn)(args...);
	}

	template <typename Ret = ptr, typename... A, typename X>
	__declspec(noinline) inline Ret this_call(X fn, A... args)
	{
		return BITCAST(Ret(__thiscall*)(A...), fn)(args...);
	}

	template <typename Ret = ptr, typename... A, typename X>
	__declspec(noinline) inline Ret fast_call(X fn, A... args)
	{
		return BITCAST(Ret(__fastcall*)(A...), fn)(args...);
	}

	template <typename Ret = ptr, typename... A, typename X>
	__declspec(noinline) inline Ret c_call(X fn, A... args)
	{
		return BITCAST(Ret(__cdecl*)(A...), fn)(args...);
	}

	template <typename Ret = ptr, typename... A, typename X>
	__declspec(noinline) inline Ret v_call(X _this, int index, A... args)
	{
		return (*BITCAST(Ret(__thiscall**)(X, A...), read<ptr>(_this) + (index * sizeof(ptr))))(_this, args...);
	}

	template <typename X>
	__declspec(noinline) inline ptr get_vtable_fn(X _this, int index)
	{
		return jc::read<ptr>(jc::read<ptr>(_this), index * sizeof(ptr));
	}
#endif

	namespace mem
	{
		template <typename T, typename... A>
		T* alloc(const std::source_location& src, A&&... args)
		{
			auto instance = new (std::nothrow) T(std::forward<A>(args)...);

			// todojc - when needed

			// save_allocation(src, typeid(T).name(), instance, sizeof(T), false);

			return instance;
		}

		template <typename T>
		void free(T* instance)
		{
			if (!instance)
				return;

			// todojc - when needed

			// save_free(instance);

			delete instance;
		}
	}
}

#ifdef JC_CLIENT
template <typename T, ptr address>
struct Singleton
{
	T* get() const noexcept { return jc::read<T*>(address); }
	T* operator*() const noexcept { return get(); }
	T* operator->() const noexcept { return get(); }

	operator bool() const noexcept { return !!get(); }
};
#endif

#define JC_ALLOC(type, ...) jc::mem::alloc<type>(std::source_location::current(), __VA_ARGS__)
#define JC_FREE jc::mem::free
#define RET_ADDRESS ptr(_ReturnAddress())