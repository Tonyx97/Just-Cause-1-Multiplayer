#include <defs/standard.h>

#include "motion_state.h"

#include <game/sys/resource/physics.h>

void hkMotionState::set_position(const vec3& v, bool absolute)
{
	if (absolute)
		jc::write(v - g_physics->get_world_position(), this, jc::hk::motion_state::TRANSLATION);
	else jc::write(v, this, jc::hk::motion_state::TRANSLATION);
}

vec3 hkMotionState::get_position() const
{
	return jc::read<vec3>(this, jc::hk::motion_state::TRANSLATION);
}