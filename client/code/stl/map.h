#pragma once

#define MAP_BASE_DEFINED

namespace jc::stl
{
	template <typename K>
	struct map_key
	{
		K key;
		K _;

		map_key(const K& k)
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
		void insert_impl(const K& key, const T& value)
		{
			const auto k = map_key(key);

			jc::this_call(M::INSERT(), this, &k, &value);
		}

	public:

		map()	{ jc::this_call(M::CREATE(), this); }
		~map()	{ jc::this_call(M::DESTROY(), this); }
	};

	template <typename M, typename K, typename T>
	struct unordered_map
	{
	public:

		struct Node
		{
			Node* left,
				* right,
				* parent;

			K key;

			T value;

			bool unk2,
				 is_leaf;
		};

	private:

		void* base;

		Node* root;

		int size;

	public:

		unordered_map()		{ jc::this_call(M::CREATE(), this); }
		~unordered_map()	{ jc::this_call(M::DESTROY(), this); }

		template <typename T>
		void _insert(const K& key, const T& value)
		{
			jc::this_call(M::INSERT(), this, &key, &value);
		}

		Node* find(const K& key)
		{
			T out;

			const auto node = *jc::this_call<Node**>(M::FIND(), this, &out, &key);

			return node != root ? node : nullptr;
		}

		Node* get_root() const { return root; }

		int get_size() const { return size; }
	};
}