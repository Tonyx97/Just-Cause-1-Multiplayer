#pragma once

namespace jc::stl
{
	template <class T>
	struct vector
	{
		char pad[0x4];

		T* first,
		 * last;

		size_t size() const		{ return last - first; }

		auto begin() const		{ return first; }
		auto end() const		{ return last; }

		T operator[] (int i) const { return begin() + i; }
	};
}