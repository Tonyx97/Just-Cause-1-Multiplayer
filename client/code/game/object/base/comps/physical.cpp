#include <defs/standard.h>

#include "physical.h"

void Physical::vcall_unk1()
{
	jc::v_call(this, jc::physical::vt::_UNK1);
}

void Physical::vcall_unk2()
{
	jc::v_call(this, jc::physical::vt::_UNK2);
}

void Physical::set_velocity(const vec3& v)
{
	jc::v_call(this, jc::physical::vt::SET_VELOCITY);
}

vec3 Physical::get_velocity() const
{
	return *jc::v_call<vec3*>(this, jc::physical::vt::GET_VELOCITY);
}