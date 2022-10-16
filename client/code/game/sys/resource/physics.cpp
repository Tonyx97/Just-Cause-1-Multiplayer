#include <defs/standard.h>

#include "physics.h"
#include "resource_streamer.h"

#include <game/object/force_pulse/force_pulse.h>
#include <game/object/asset/asset_pfx.h>

namespace jc::physics
{
	std::unordered_map<std::string, bref<AssetPFX>> pfxs;
}

using namespace jc::physics;

void hkWorld::set_gravity(const vec3& v)
{
	jc::write(vec4(v, 1.f), this, jc::hk_world::GRAVITY);
}

vec3 hkWorld::get_gravity() const
{
	return jc::read<vec3>(this, jc::hk_world::GRAVITY);
}

void Physics::init()
{
}

void Physics::destroy()
{
}

void Physics::create_force_pulse(const vec3& position, float radius, float force, float damage_multiplier)
{
	ForcePulse pulse;

	pulse.set_position(position);
	pulse.set_radius(vec3(radius));
	pulse.set_force(force);
	pulse.set_damage(damage_multiplier);
	pulse.activate();
}

bool Physics::load_pfx(const std::string& filename)
{
	const auto file_data = util::fs::read_bin_file(filename);

	if (file_data.empty())
		return false;

	return load_pfx(filename, file_data);
}

bool Physics::load_pfx(const std::string& filename, const std::vector<uint8_t>& data)
{
	AssetDataHolder data_holder(data);

	jc::stl::string name = util::fs::strip_parent_path(filename);

	bref<AssetPFX> r(true);

	mat4 identity = mat4(1.f);

	jc::this_call(fn::LOAD_PFX_FROM_MEM, this, &name, &r, &identity, true, false, &data_holder);

	pfxs.insert({ filename, std::move(r) });

	return true;
}

bool Physics::unload_pfx(const std::string& filename)
{
	auto it = pfxs.find(filename);
	if (it == pfxs.end())
		return false;

	pfxs.erase(it);

	return true;
}

bool Physics::raycast(const vec3& origin, const vec3& dest, ray_hit_info& hit_info, bool unk1, bool unk2)
{
	uint8_t buffer[0x54];

	jc::this_call<int, void*>(fn::SETUP_RAYCAST_CTX_BASIC, buffer);

	const auto direction = dest - origin;

	ray r(origin, direction);

	return !!jc::this_call<void*, Physics*, const ray*, int, float, ray_hit_info*, void*, bool, bool>(
		fn::RAYCAST,
		this,
		&r,
		0,
		glm::length(direction),
		&hit_info,
		buffer,
		unk1,
		unk2);
}

hkWorld* Physics::get_hk_world() const
{
	return jc::read<hkWorld*>(this, HK_WORLD);
}

vec3 Physics::get_world_position() const
{
	return jc::read<vec3>(this, WORLD_POSITION);
}

vec3 Physics::get_gravity() const
{
	const auto world = get_hk_world();
	return world ? world->get_gravity() : vec3 {};
}