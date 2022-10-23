#pragma once

namespace jc::resource_cache
{
	namespace vt
	{
	}
}

template <typename T, ptr cache_offset>
struct ResourceCache
{
	struct cache_map : public jc::stl::unordered_map<cache_map, jc::stl::string, shared_ptr<T>>
	{
		static constexpr auto FIND() { return 0x659FF0; }
	};

	cache_map* get_cache() { return REF(cache_map*, this, cache_offset); }

	shared_ptr<T> get_cache_item(const jc::stl::string& name)
	{
		if (auto node = get_cache()->find(name))
			return node->value;

		return {};
	}

	bool has_resource(const std::string& name) { return !!get_cache()->find(name); }
};