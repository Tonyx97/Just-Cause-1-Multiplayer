#pragma once

#define MAP_BASE_DEFINED

namespace jc
{
	template <typename K>
	struct map_key
	{
		K key;
		K _;

		map_key(K k)
			: key(k)
			, _(K())
		{
		}
	};

	template <class M, typename K>
	struct map
	{
	public:
		void* base;
		void* data;

		int size;

		template <typename T>
		void insert_impl(K key, const T& value)
		{
			const auto k = jc::map_key(key);

			jc::this_call(M::INSERT(), this, &k, &value);
		}

	public:
		map()
		{
			jc::this_call(M::CREATE(), this);
		}

		~map()
		{
			jc::this_call(M::DESTROY(), this);
		}
	};
}