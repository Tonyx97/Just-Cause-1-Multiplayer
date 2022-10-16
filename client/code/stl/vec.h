#pragma once

namespace jc::stl
{
	namespace _vec
	{
		static constexpr uint32_t VECTOR_POINTER_ERASE = 0x4D3C50;
	}

	template <class T>
	struct vector
	{
		char pad[0x4];

		T* first,
		 * last;

		size_t size() const		{ return last - first; }

		auto begin() const		{ return first; }
		auto end() const		{ return last; }

		auto find(T object) const
		{
			for (auto it = begin(); it != end(); ++it)
				if (*it == object)
					return it;

			return end();
		}

		T operator[] (int i) const
		{
			check(i >= 0 && i < static_cast<int>(size()), "{} out of bounds access", typeid(*this).name());

			return *(begin() + i);
		}
	};

	template <class T>
	struct vector_ptr : public vector<T>
	{
		void erase(T* it) { int out = 0; jc::this_call(_vec::VECTOR_POINTER_ERASE, this, &out, it); }
	};
}