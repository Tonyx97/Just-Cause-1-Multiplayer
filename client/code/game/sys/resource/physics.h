#pragma once

#include <game/object/physics/pfx_instance.h>

#include <havok/defs.h>

#include "resource_cache.h"

namespace jc::physics
{
	static constexpr uint32_t SINGLETON = 0xD37340;		// Physics*

	static constexpr uint32_t WORLD_POSITION = 0x2C;	// vec3
	static constexpr uint32_t PFX_CACHE		 = 0x40;	// map of pfxs
	static constexpr uint32_t HK_WORLD		 = 0x4C;	// hkWorld*

	namespace fn
	{
		static constexpr uint32_t FIND_PFX_INSTANCE			= 0x659FF0;
		static constexpr uint32_t LOAD_PFX_FROM_MEM			= 0x4E4760;
		static constexpr uint32_t RAY_FILTER_CTOR			= 0x4CAFF0;
		static constexpr uint32_t RAYCAST					= 0x4E56D0;
	}
}

namespace jc::hk_world
{
	static constexpr uint32_t GRAVITY = 0xE0; // vec4
}

struct ray
{
	vec3 origin,
		 direction;

	ray(const vec3& origin, const vec3& direction)
		: origin(origin)
		, direction(glm::normalize(direction))
	{
	}
};

struct ray_hit_info_internal
{
	void* object = nullptr;
	void* rigidbody = nullptr;

	vec3 normal;

	float distance_factor;
};

struct ray_hit_info
{
	void* object = nullptr;

	vec3 hit_position,
		 normal;

	float distance;
};

struct ray_filter
{
	float early_hit_out_fraction;
	const class hkpCollidable* root_collidable;
	vec4 normal;
	float hit_fraction;
	int extra_info;
	int unk[2];
	hkpShapeKey shape_keys[hkpShapeRayCastOutput_MAX_HIERARCHY_DEPTH];
	int shape_key_index;
	int ignore_mask;
	int force_mask;
	PfxInstance* ignore_pfx_instance;
	const class hkpShape* shape;
	hkpShapeKey _shape_keys[4];

	ray_filter() { jc::this_call<int>(jc::physics::fn::RAY_FILTER_CTOR, this); }
};

struct hkWorld
{
	void set_gravity(const vec3& v);

	vec3 get_gravity() const;
};

class Physics : public ResourceCache<PfxInstance, jc::physics::PFX_CACHE>
{
public:

	void init();
	void destroy();

	void create_force_pulse(const vec3& position, float radius, float force, float damage_multiplier = 0.f);
	void set_gravity(const vec3& v);

	bool load_pfx(const std::string& filename);
	bool load_pfx(const std::string& filename, const std::vector<uint8_t>& data);
	bool unload_pfx(const std::string& filename);
	bool raycast(const vec3& origin, const vec3& dest, ray_hit_info& hit_info, bool force_better_heights = false, bool hit_characters = true);

	hkWorld* get_hk_world() const;

	vec3 get_world_position() const;
	vec3 get_gravity() const;
};

inline Singleton<Physics, jc::physics::SINGLETON> g_physics;