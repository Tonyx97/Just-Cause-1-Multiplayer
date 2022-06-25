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
	Transform t(position);

	return BITCAST(void*(__thiscall*)(void*, const std::string*, Transform*, bool, bool), jc::particle_system::fn::SPAWN_FX)(this, &name, &t, unk1, unk2);
}