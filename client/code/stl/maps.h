#pragma once

#define MAP_BASE_DEFINED

enum ValueType
{
	ValueType_Bool = 1,
	ValueType_Int = ValueType_Bool,
	ValueType_Int16 = 2,
	ValueType_Float = ValueType_Int16,
	ValueType_String = 3,
	ValueType_Vec3 = 5,
	ValueType_Mat4 = 8,
	ValueType_Unknown = 0
};

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