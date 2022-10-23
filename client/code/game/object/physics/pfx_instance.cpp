#include <defs/standard.h>

#include "pfx_instance.h"

void PfxInstance::vcall_unk1()
{
	jc::v_call(instance.get(), jc::pfx_instance::vt::_UNK1);
}

void PfxInstance::vcall_unk2()
{
	jc::v_call(instance.get(), jc::pfx_instance::vt::_UNK2);
}

void PfxInstance::set_velocity(const vec3& v)
{
	jc::v_call(instance.get(), jc::pfx_instance::vt::SET_VELOCITY, &v);
}

void PfxInstance::set_transform(const Transform& v)
{
	jc::v_call(instance.get(), jc::pfx_instance::vt::SET_TRANSFORM, &v);
}

vec3 PfxInstance::get_velocity() const
{
	vec3 out;

	return *jc::v_call<vec3*>(instance.get(), jc::pfx_instance::vt::GET_VELOCITY, &out);
}