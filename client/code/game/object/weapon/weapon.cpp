#include <defs/standard.h>

#include "weapon.h"

#include <game/transform/transform.h>

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

void WeaponInfo::set_accuracy(bool ai, float v)
{
	if (ai)
		jc::write(v, this, jc::weapon_info::ACCURACY_AI);
	else jc::write(v, this, jc::weapon_info::ACCURACY_PLAYER);
}

void WeaponInfo::set_infinite_ammo(bool v)
{
	jc::write(v, this, jc::weapon_info::INFINITE_AMMO);
}

bool WeaponInfo::can_create_shells() const
{
	return jc::read<bool>(this, jc::weapon_info::CAN_CREATE_SHELLS);
}

bool WeaponInfo::is_vehicle_weapon() const
{
	return get_icon_id() == 0;
}

uint8_t WeaponInfo::get_id() const
{
	return static_cast<uint8_t>(jc::read<int32_t>(this, jc::weapon_info::ID));
}

int32_t WeaponInfo::get_bullet_type() const
{
	return jc::read<int32_t>(this, jc::weapon_info::BULLET_TYPE);
}

int32_t WeaponInfo::get_type_id() const
{
	return jc::read<int32_t>(this, jc::weapon_info::TYPE_ID);
}

int32_t WeaponInfo::get_icon_id() const
{
	return jc::read<int32_t>(this, jc::weapon_info::ICON_ID);
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

float WeaponInfo::get_bullet_force1() const
{
	return jc::read<float>(this, jc::weapon_info::BULLET_FORCE1);
}

float WeaponInfo::get_bullet_force2() const
{
	return jc::read<float>(this, jc::weapon_info::BULLET_FORCE2);
}

float WeaponInfo::get_accuracy(bool ai) const
{
	return ai ? jc::read<float>(this, jc::weapon_info::ACCURACY_AI) :
				jc::read<float>(this, jc::weapon_info::ACCURACY_PLAYER);
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

void Weapon::set_last_shot_time(float v)
{
	jc::write(v, this, jc::weapon::LAST_SHOT_TIME);
}

void Weapon::set_muzzle_position(const vec3& v)
{
	jc::write(v, this, jc::weapon::MUZZLE_POSITION);
}

void Weapon::set_last_muzzle_position(const vec3& v)
{
	jc::write(v, this, jc::weapon::LAST_MUZZLE_POSITION);
}

void Weapon::set_last_muzzle_transform(const Transform& v)
{
	jc::write(v, this, jc::weapon::LAST_MUZZLE_TRANSFORM);
}

void Weapon::set_aim_target(const vec3& v)
{
	jc::write(v, this, jc::weapon::AIM_TARGET);
}

void Weapon::force_fire()
{
	jc::write(true, this, jc::weapon::CAN_FIRE);
}

void Weapon::set_enabled(bool v)
{
	jc::v_call(this, jc::weapon::vt::SET_ENABLED, v);
}

bool Weapon::is_reloading() const
{
	return jc::read<bool>(this, jc::weapon::RELOADING);
}

bool Weapon::can_fire() const
{
	return jc::read<bool>(this, jc::weapon::CAN_FIRE);
}

bool Weapon::is_trigger_pulled() const
{
	return jc::read<bool>(this, jc::weapon::TRIGGER_PULLED);
}

uint8_t Weapon::get_id() const
{
    return get_info()->get_id();
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

WeaponInfo* Weapon::get_info() const
{
	return jc::read<WeaponInfo*>(this, jc::weapon::WEAPON_INFO);
}

Character* Weapon::get_owner() const
{
	return jc::read<Character*>(this, jc::weapon::CHARACTER_OWNER);
}

vec3 Weapon::get_muzzle_position()
{
	return get_muzzle_transform()->get_position();
}

vec3 Weapon::get_aim_target()
{
	return jc::read<vec3>(this, jc::weapon::AIM_TARGET);
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