#include <defs/standard.h>

#include "weapon.h"

void WeaponInfo::set_max_mag_ammo(int32_t v)
{
	jc::write(v, this, jc::weapon_info::MAX_MAG_AMMO);
}

void WeaponInfo::set_bullets_to_fire(int32_t v)
{
	jc::write(v, this, jc::weapon_info::BULLETS_TO_FIRE);
}

void WeaponInfo::set_muzzle_offset(const vec3& v)
{
	jc::write(v, this, jc::weapon_info::MUZZLE_OFFSET);
}

void WeaponInfo::set_fire_sound_id(int32_t v)
{
	jc::write(v, this, jc::weapon_info::FIRE_SOUND_ID);
}

void WeaponInfo::set_bullet_force1(float v)
{
	jc::write(v, this, jc::weapon_info::BULLET_FORCE1);
}

void WeaponInfo::set_bullet_force2(float v)
{
	jc::write(v, this, jc::weapon_info::BULLET_FORCE2);
}

void WeaponInfo::set_can_create_shells(bool v)
{
	jc::write(v, this, jc::weapon_info::CAN_CREATE_SHELLS);
}

bool WeaponInfo::can_create_shells()
{
	return jc::read<bool>(this, jc::weapon_info::CAN_CREATE_SHELLS);
}

int32_t WeaponInfo::get_slot()
{
	return jc::read<int32_t>(this, jc::weapon_info::SLOT);
}

int32_t WeaponInfo::get_max_mag_ammo()
{
	return jc::read<int32_t>(this, jc::weapon_info::MAX_MAG_AMMO);
}

int32_t WeaponInfo::get_bullets_to_fire()
{
	return jc::read<int32_t>(this, jc::weapon_info::BULLETS_TO_FIRE);
}

int32_t WeaponInfo::get_fire_sound_id()
{
	return jc::read<int32_t>(this, jc::weapon_info::FIRE_SOUND_ID);
}

float WeaponInfo::get_bullet_force1()
{
	return jc::read<float>(this, jc::weapon_info::BULLET_FORCE1);
}

float WeaponInfo::get_bullet_force2()
{
	return jc::read<float>(this, jc::weapon_info::BULLET_FORCE2);
}

vec3 WeaponInfo::get_muzzle_offset()
{
	return jc::read<vec3>(this, jc::weapon_info::MUZZLE_OFFSET);
}

const char* WeaponInfo::get_type_name()
{
	return REF(const char*, this, jc::weapon_info::TYPE_NAME);
}

const char* WeaponInfo::get_name()
{
	return REF(const char*, this, jc::weapon_info::NAME);
}

void Weapon::set_ammo(int32_t v)
{
	jc::write(v, this, jc::weapon::AMMO);
}

bool Weapon::is_firing()
{
	return jc::read<bool>(this, jc::weapon::FIRING);
}

int32_t Weapon::get_ammo()
{
	return jc::read<int32_t>(this, jc::weapon::AMMO);
}

int32_t Weapon::get_bullets_fired()
{
	return jc::read<int32_t>(this, jc::weapon::TOTAL_BULLETS_FIRED);
}

int32_t Weapon::get_bullets_fired_now()
{
	return jc::read<int32_t>(this, jc::weapon::TOTAL_BULLETS_FIRED_NOW);
}

float Weapon::get_last_shot_time()
{
	return jc::read<float>(this, jc::weapon::LAST_SHOT_TIME);
}

WeaponInfo* Weapon::get_info()
{
	return jc::read<WeaponInfo*>(this, jc::weapon::WEAPON_INFO);
}

Character* Weapon::get_owner()
{
	return jc::read<Character*>(this, jc::weapon::CHARACTER_OWNER);
}

vec3 Weapon::get_aim_hit_position()
{
	return jc::read<vec3>(this, jc::weapon::AIM_HIT_POSITION);
}

Transform* Weapon::get_grip_transform()
{
	return REF(Transform*, this, jc::weapon::GRIP_TRANSFORM);
}

Transform* Weapon::get_last_muzzle_transform()
{
	return REF(Transform*, this, jc::weapon::LAST_MUZZLE_TRANSFORM);
}

Transform* Weapon::get_muzzle_transform()
{
	return REF(Transform*, this, jc::weapon::MUZZLE_TRANSFORM);
}

Transform* Weapon::get_last_ejector_transform()
{
	return REF(Transform*, this, jc::weapon::LAST_EJECTOR_TRANSFORM);
}