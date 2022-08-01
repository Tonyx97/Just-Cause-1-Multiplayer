#include <defs/standard.h>

#include "pfx_base.h"

void PfxBase::vcall_unk1()
{
	jc::v_call(this, jc::pfx_base::vt::_UNK1);
}

void PfxBase::vcall_unk2()
{
	jc::v_call(this, jc::pfx_base::vt::_UNK2);
}

void PfxBase::set_velocity(const vec3& v)
{
	jc::v_call(this, jc::pfx_base::vt::SET_VELOCITY, &v);
}

vec3 PfxBase::get_velocity() const
{
	vec3 out;

	return *jc::v_call<vec3*>(this, jc::pfx_base::vt::GET_VELOCITY, &out);
}