#pragma once

using ptr = uintptr_t;

#define RCAST(T, x) reinterpret_cast<T>(x)
#define BITCAST(T, x) std::bit_cast<T>(x)
#define REF(T, x, off) std::bit_cast<T>(ptr(x) + off)
#define REFNO(T, x) std::bit_cast<T>(ptr(x))

#ifdef JC_CLIENT
struct patch_value
{
	ptr	   address;
	size_t size;

	patch_value(ptr address, size_t size)
		: address(address)
		, size(size)
	{
	}
};
#endif