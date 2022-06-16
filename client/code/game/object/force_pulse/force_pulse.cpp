#include <defs/standard.h>

#include "force_pulse.h"

ForcePulse::ForcePulse()
{
	jc::this_call<ForcePulse*>(jc::force_pulse::fn::CREATE, this);
}

ForcePulse::~ForcePulse()
{
	jc::this_call<ForcePulse*>(jc::force_pulse::fn::DESTROY, this);
}

void ForcePulse::set_position(const vec3& v)
{
	jc::this_call<Transform*>(jc::force_pulse::fn::SET_POSITION, this, &v);
}

void ForcePulse::set_force(float v)
{
	jc::this_call<int>(jc::force_pulse::fn::SET_FORCE, this, v);
}

void ForcePulse::set_radius(const vec3& v)
{
	jc::this_call<Transform*>(jc::force_pulse::fn::SET_RADIUS, this, &v);
}

void ForcePulse::set_damage(float v)
{
	jc::this_call<int>(jc::force_pulse::fn::SET_DAMAGE, this, v);
}

void ForcePulse::activate()
{
	jc::this_call<ForcePulse*>(jc::force_pulse::fn::ACTIVATE, this);
}