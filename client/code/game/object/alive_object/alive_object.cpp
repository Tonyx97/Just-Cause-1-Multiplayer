#include <defs/standard.h>

#include "alive_object.h"

void AliveObject::set_hp(float v)
{
	jc::v_call<AliveObject*>(this, jc::alive_object::vt::SET_HEALTH, v);
}

void AliveObject::set_max_hp(float v)
{
	jc::write(v, this, jc::alive_object::MAX_HEALTH);
}

bool AliveObject::is_alive() const
{
	return get_real_hp() > 0.f;
}

float AliveObject::get_real_hp() const
{
	return jc::read<float>(this, jc::alive_object::HEALTH);
}

float AliveObject::get_max_hp() const
{
	return jc::read<float>(this, jc::alive_object::MAX_HEALTH);
}

float AliveObject::get_hp() const
{
	if (!is_alive())
		return 0.f;

	const auto current_health = get_real_hp() > get_max_hp() ? get_max_hp() : get_real_hp();

	return (current_health / get_max_hp()) * 100.f;
}