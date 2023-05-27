#include <defs/standard.h>

#include "particle_system.h"

#include <game/transform/transform.h>

void ParticleSystem::init()
{
}

void ParticleSystem::destroy()
{
}

void* ParticleSystem::spawn_fx(const std::string& name, const vec3& position, const vec3& rotation, const vec3& velocity, bool unk1, bool unk2)
{
	Transform transform(position);

	const jc::stl::string str = name;

	transform.rotate(radians(rotation));
	transform.scale(velocity);

	return jc::this_call<void*>(jc::particle_system::fn::SPAWN_FX, this, &str, &transform, unk1, unk2);
}