#pragma once

namespace jc::reference
{
	namespace fn
	{
		static constexpr ptr INC_REF = 0x40E6F0;
		static constexpr ptr DEC_REF = 0x40E840;
		static constexpr ptr UNK_REF = 0x41F890;
	}
}

namespace jc::big_reference
{
	namespace fn
	{
		static constexpr ptr DEC_REF = 0x4C45F0;
		static constexpr ptr INIT	 = 0x4D8310;
	}
}

template <typename T>
struct ref_count
{
	void* vt;
	int	  uses, weaks;
	int	  unk1;
	T*	  obj;
	int	  unk2;
};

template <typename T>
struct ref_holder
{
	T*			  obj;
	ref_count<T>* counter;
};

template <typename T>
struct ref
{
	T*			  obj	  = nullptr;
	ref_count<T>* counter = nullptr;

	ref() {}

	template <typename X, typename Y>
	ref(X object, Y fn)
	{
		jc::this_call(fn, &counter, object, false);

		obj = counter->obj;
	}

	// avoid copies

	ref(const ref&) = delete;

	// force copy elision

	ref(ref&& other)
	{
		obj		= std::exchange(other.obj, nullptr);
		counter = std::exchange(other.counter, nullptr);
	}

	// implement move operator

	ref& operator=(ref&& other)
	{
		obj		= std::exchange(other.obj, nullptr);
		counter = std::exchange(other.counter, nullptr);

		return *this;
	}

	~ref()
	{
		if (!obj || !counter)
			return;

		dec();
	}

	void dec()
	{
		bool destroyed = counter->uses == 1;

		jc::this_call(jc::reference::fn::DEC_REF, counter);

		if (destroyed)
		{
			obj		= nullptr;
			counter = nullptr;
		}
	}

	T* operator->() const noexcept { return obj; }
	T* operator*() const noexcept { return obj; }

	/**
	* I don't really know wtf this is but it's very important, we should call it 
	* before making any object active/destroying it
	*/
	void make_valid()
	{
		jc::this_call(jc::reference::fn::UNK_REF, counter);
	}

	operator bool () const { return !!obj; }

	ref_holder<T> to_holder() const
	{
		return { obj, counter };
	}

	T* move_to(auto& container)
	{
		const auto item = obj;

		container.push_back(std::move(*this));

		return item;
	}

	T* move_to_set(auto& container)
	{
		const auto item = obj;

		container.insert(std::move(*this));

		return item;
	}
};

template <typename T>
struct bref
{
	void*		  vt	  = nullptr;
	T*			  obj	  = nullptr;
	ref_count<T>* counter = nullptr;
	void*		  unk2	  = nullptr;
	void*		  unk3	  = nullptr;
	void*		  unk4	  = nullptr;

	bref() {}
	~bref()
	{
		if (!vt || !obj || !counter || !unk2 || !unk3 || unk4)
			return;

		dec();
	}

	// avoid copies

	bref(const bref&) = delete;

	// force copy elision

	bref(bref&& other)
	{
		memcpy(this, &other, sizeof(*this));
		memset(&other, 0, sizeof(*this));
	}

	// implement move operator

	bref& operator=(bref&& other)
	{
		memcpy(this, &other, sizeof(*this));
		memset(&other, 0, sizeof(*this));

		return *this;
	}

	void dec()
	{
		bool destroyed = counter->uses == 1;

		jc::this_call(jc::big_reference::fn::DEC_REF, this);

		if (destroyed)
			memset(this, 0, sizeof(*this));
	}

	T* operator->() const noexcept { return obj; }
	T* operator*() const noexcept { return obj; }
};