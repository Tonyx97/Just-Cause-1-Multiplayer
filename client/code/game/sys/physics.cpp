#include <defs/standard.h>

#include "physics.h"

#include <game/object/force_pulse/force_pulse.h>

void hkWorld::set_gravity(const vec4& v)
{
	jc::write(v, this, jc::hk_world::GRAVITY);
}

vec4 hkWorld::get_gravity() const
{
	return jc::read<vec4>(this, jc::hk_world::GRAVITY);
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

bool Physics::raycast(const vec3& origin, const vec3& dest, ray_hit_info& hit_info, bool unk1, bool unk2)
{
	uint8_t buffer[0x54];

	jc::this_call<int, void*>(jc::physics::fn::SETUP_RAYCAST_CTX_BASIC, buffer);

	const auto direction = dest - origin;

	ray r(origin, direction);

	return !!jc::this_call<void*, Physics*, const ray*, int, float, ray_hit_info*, void*, bool, bool>(
		jc::physics::fn::RAYCAST,
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
	return jc::read<hkWorld*>(this, jc::physics::HK_WORLD);
}

vec3 Physics::get_world_position() const
{
	return jc::read<vec3>(this, jc::physics::WORLD_POSITION);
}