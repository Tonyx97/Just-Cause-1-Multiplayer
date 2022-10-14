#pragma once

namespace jc::resource_cache
{
	namespace vt
	{
	}
}

struct resource_cache_map : public jc::stl::unordered_map<resource_cache_map, jc::stl::string, ref<ptr>>
{
	static constexpr auto FIND() { return 0x659FF0; }
};

template <ptr cache_offset>
struct ResourceCache
{
	resource_cache_map* get_cache() const { return REF(resource_cache_map*, this, cache_offset); }

	bool has_resource(const std::string& name) const { return !!get_cache()->find(name); }
};