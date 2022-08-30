#pragma once

#include "../base/base.h"

namespace jc::weapon
{
	static constexpr uint32_t GRIP_TRANSFORM		  = 0x38; // Transform
	static constexpr uint32_t CHARACTER_OWNER		  = 0x78; // Character*
	static constexpr uint32_t LAST_MUZZLE_TRANSFORM	  = 0xC0; // Transform
	static constexpr uint32_t LAST_MUZZLE_POSITION	  = LAST_MUZZLE_TRANSFORM + 0x30; // Transform
	static constexpr uint32_t MUZZLE_TRANSFORM		  = 0x100; // Transform
	static constexpr uint32_t MUZZLE_POSITION		  = MUZZLE_TRANSFORM + 0x30; // vec3
	static constexpr uint32_t LAST_EJECTOR_TRANSFORM  = 0x140; // Transform
	static constexpr uint32_t AMMO					  = 0x184; // int32_t
	static constexpr uint32_t CAN_FIRE				  = 0x188; // bool
	static constexpr uint32_t RELOADING				  = 0x18A; // bool
	static constexpr uint32_t LAST_SHOT_TIME		  = 0x18C; // float
	static constexpr uint32_t LAST_SHOT_TIME2		  = 0x19C; // float
	static constexpr uint32_t AIM_TARGET			  = 0x1A0; // vec3
	static constexpr uint32_t WEAPON_INFO			  = 0x1AC; // WeaponInfo*
	static constexpr uint32_t TOTAL_BULLETS_FIRED	  = 0x1C4; // int32_t
	static constexpr uint32_t TOTAL_BULLETS_FIRED_NOW = 0x1C8; // int32_t
	static constexpr uint32_t TRIGGER_PULLED		  = 0x1CC; // bool

	namespace vt
	{
		static constexpr uint32_t SET_ENABLED		= 36;	// todojc - this is from Item, once the class is done, move it there
	}
}

namespace jc::weapon_info
{
	static constexpr uint32_t ID				= 0x0; // int32_t
	static constexpr uint32_t ICON_ID			= 0x4; // int32_t
	static constexpr uint32_t TYPE_ID			= 0xC; // int32_t
	static constexpr uint32_t BULLET_TYPE		= 0x10; // int
	static constexpr uint32_t MAX_MAG_AMMO		= 0x38; // int
	static constexpr uint32_t BULLETS_TO_FIRE	= 0x44; // int
	static constexpr uint32_t ACCURACY_AI		= 0x48; // float
	static constexpr uint32_t ACCURACY_PLAYER	= 0x4C; // float
	static constexpr uint32_t TYPE_NAME			= 0x54; // char[28]
	static constexpr uint32_t NAME				= 0x54; // char[28]
	static constexpr uint32_t MUZZLE_OFFSET		= 0xA4; // vec3
	static constexpr uint32_t FIRE_SOUND_ID		= 0x118; // int
	static constexpr uint32_t BULLET_FORCE1		= 0x130; // float
	static constexpr uint32_t BULLET_FORCE2		= 0x134; // float
	static constexpr uint32_t CAN_CREATE_SHELLS = 0x176; // bool
	static constexpr uint32_t INFINITE_AMMO		= 0x17A; // bool
}

class Character;
class Transform;

class WeaponInfo
{
public:
	void set_max_mag_ammo(int32_t v);
	void set_bullets_to_fire(int32_t v);
	void set_muzzle_offset(const vec3& v);
	void set_fire_sound_id(int32_t v);
	void set_bullet_force1(float v);
	void set_bullet_force2(float v);
	void set_can_create_shells(bool v);
	void set_accuracy(bool ai, float v);
	void set_infinite_ammo(bool v);

	bool can_create_shells() const;
	bool is_vehicle_weapon() const;

	uint8_t get_id() const;

	int32_t get_bullet_type() const;
	int32_t get_type_id() const;
	int32_t get_icon_id() const;
	int32_t get_max_mag_ammo();
	int32_t get_bullets_to_fire();
	int32_t get_fire_sound_id();

	float get_bullet_force1() const;
	float get_bullet_force2() const;
	float get_accuracy(bool ai) const;

	vec3 get_muzzle_offset();

	const char* get_type_name();
	const char* get_name();
};

class Weapon : public ObjectBase
{
public:
	void set_ammo(int32_t v);
	void set_last_shot_time(float v);
	void set_muzzle_position(const vec3& v);
	void set_last_muzzle_position(const vec3& v);
	void set_last_muzzle_transform(const Transform& v);
	void set_aim_target(const vec3& v);
	void force_fire();
	void set_enabled(bool v);

	bool is_reloading() const;
	bool can_fire() const;
	bool is_trigger_pulled() const;

	uint8_t get_id() const;

	int32_t get_ammo();
	int32_t get_bullets_fired();
	int32_t get_bullets_fired_now();

	float get_last_shot_time();

	WeaponInfo* get_info() const;
	Character*	get_owner() const;

	vec3 get_aim_target();

	Transform* get_grip_transform();
	Transform* get_last_muzzle_transform();
	Transform* get_muzzle_transform();
	Transform* get_last_ejector_transform();
};