#include <defs/standard.h>

#include "alive_object.h"

void AliveObject::set_alive_flag(uint16_t v)
{
	jc::write(get_alive_flags() | v, this, jc::alive_object::FLAGS);
}

void AliveObject::remove_alive_flag(uint16_t v)
{
	jc::write(get_alive_flags() & ~v, this, jc::alive_object::FLAGS);
}

void AliveObject::set_hp(float v)
{
	jc::v_call<AliveObject*>(this, jc::alive_object::vt::SET_HEALTH, v);
}

void AliveObject::set_max_hp(float v)
{
	jc::write(v, this, jc::alive_object::MAX_HEALTH);
}

void AliveObject::set_invincible(bool v)
{
	if (v)
		set_alive_flag(AliveObjectFlag_Invincible);
	else remove_alive_flag(AliveObjectFlag_Invincible);
}

bool AliveObject::is_alive() const
{
	return get_real_hp() > 0.f;
}

bool AliveObject::is_invincible() const
{
	return get_alive_flags() & AliveObjectFlag_Invincible;
}

uint16_t AliveObject::get_alive_flags() const
{
	return jc::read<uint16_t>(this, jc::alive_object::FLAGS);
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