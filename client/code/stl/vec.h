#pragma once

namespace jc::stl
{
	template <class T>
	struct vector
	{
		static_assert(std::is_pointer_v<T>, "This vector class must handle pointers only");

		char pad[0x4];

		T first,
		  last;

		size_t size() const		{ return last - first; }

		auto begin() const		{ return (T*)first; }
		auto end() const		{ return (T*)last; }

		T operator[] (int i) const { return begin() + i; }
	};
}