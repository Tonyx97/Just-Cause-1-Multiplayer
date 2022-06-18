#pragma once

namespace jc::weapon
{
	static constexpr uint32_t GRIP_TRANSFORM		  = 0x38; // Transform
	static constexpr uint32_t CHARACTER_OWNER		  = 0x78; // Character*
	static constexpr uint32_t LAST_MUZZLE_TRANSFORM	  = 0xC0; // Transform
	static constexpr uint32_t MUZZLE_TRANSFORM		  = 0x100; // Transform
	static constexpr uint32_t LAST_EJECTOR_TRANSFORM  = 0x140; // Transform
	static constexpr uint32_t AMMO					  = 0x184; // int32_t
	static constexpr uint32_t LAST_SHOT_TIME		  = 0x19C; // float
	static constexpr uint32_t AIM_HIT_POSITION		  = 0x1A0; // vec3
	static constexpr uint32_t WEAPON_INFO			  = 0x1AC; // WeaponInfo*
	static constexpr uint32_t TOTAL_BULLETS_FIRED	  = 0x1C4; // int32_t
	static constexpr uint32_t TOTAL_BULLETS_FIRED_NOW = 0x1C8; // int32_t
	static constexpr uint32_t FIRING				  = 0x1CC; // bool
}

namespace jc::weapon_info
{
	static constexpr uint32_t SLOT				= 0x10; // int
	static constexpr uint32_t MAX_MAG_AMMO		= 0x38; // int
	static constexpr uint32_t BULLETS_TO_FIRE	= 0x44; // int
	static constexpr uint32_t TYPE_NAME			= 0x54; // char[28]
	static constexpr uint32_t NAME				= 0x54; // char[28]
	static constexpr uint32_t MUZZLE_OFFSET		= 0xA4; // vec3
	static constexpr uint32_t FIRE_SOUND_ID		= 0x118; // int
	static constexpr uint32_t BULLET_FORCE1		= 0x130; // float
	static constexpr uint32_t BULLET_FORCE2		= 0x134; // float
	static constexpr uint32_t CAN_CREATE_SHELLS = 0x176; // bool
}

class Character;
class Transform;

enum WeaponSlot
{
	WeaponSlot_A,
	WeaponSlot_B,
	WeaponSlot_C,
	WeaponSlot_D,
	WeaponSlot_E,
	WeaponSlot_F,
	WeaponSlot_G,
	WeaponSlot_H,
};

enum WeaponID
{
	Weapon_None,
	Weapon_Pistol = 1, // MORETTI P-94
	Weapon_Assault_Rifle = 2, // ADLER FF M-72
	Weapon_Shotgun_pump_action = 3, // MORETTI CCW CENTURION
	Weapon_M60 = 4, // M60
	Weapon_Mounted_M60 = 5, // MOUNTED M60
	Weapon_Rocket_Launcher = 6, // MAKO OCRA
	Weapon_Helicopter_gun = 7, // HELICOPTER GUN
	Weapon_Tank_Cannon = 8, // TANK CANNON
	Weapon_Signature_Gun = 9, // HOLDT R4 PITBULLS
	Weapon_Master_Signature_Gun = 10, // HOLDT R4 PITBULLS
	Weapon_Silenced_Pistol = 11, // GALLINI M39 "HUSHER"
	Weapon_Revolver = 12, // HARKER 357 SAWBACK
	Weapon_1H_SMG = 13, // AVIV SCARAB
	Weapon_Silenced_SMG = 14, // NOVA 9
	Weapon_2H_SMG = 15, // AVIV PANTHERA
	Weapon_Sawed_off_shotgun = 16, // DAWSON 610
	Weapon_Grenade_Launcher = 17, // MAKO HAMMERHEAD
	Weapon_Disposable_RPG = 18, // RPG M-112
	Weapon_Sniper_rifle = 19, // BARCLAY PHANTOM
	Weapon_Assault_rifle_sniper = 20, // LANCE FDL
	Weapon_Assault_Rifle_heavy = 21, // FLEISHER AR-5 BLIZZARD
	Weapon_Assault_Rifle_high_tech = 22, // HASWELL GEN2 VINDICATOR
	Weapon_Shotgun_automatic = 23, // MORETTI CCW PRAETORIAN
	Weapon_Boat_Gun = 24, // BOAT GUN
	Weapon_AAGun = 25, // AAGUN
	Weapon_LAVE_055_Tank_Cannon = 26, // LAVE_055 TANK CANNON
	Weapon_LAVE_014_Tank_Cannon = 27, // LAVE_014 TANK CANNON
	Weapon_FFAR_Rockets = 28, // FFAR ROCKETS
	Weapon_Hellfire = 29, // HELLFIRE
	Weapon_SIDEWINDER = 30, // SIDEWINDER
	Weapon_AIRPLANE_GUNS = 31, // AIRPLANE_GUNS
	Weapon_LAVE_055_AAGUN = 32, // LAVE_055_AAGUN
	Weapon_LAVE_055_ROCKETS = 33, // LAVE_055_ROCKETS
	Weapon_Sheldons_M60 = 34, // SHELDONS M60
	Weapon_LAVE_014_ROCKETS = 35, // LAVE_014_ROCKETS
	Weapon_LAVE_014_AAGUN = 36, // LAVE_014_AAGUN
	Weapon_Grapplinghook = 37, // PROTEC GRAPPLER G3
	Weapon_Car_guns = 38, // CAR GUNS
	Weapon_Boat_cannon = 39, // BOAT CANNON
	Weapon_Mounted_GL = 40, // MOUNTED GL
	Weapon_Timed_Explosive = 41, // TIMED EXPLOSIVES
	Weapon_Triggered_Explosive = 42, // TRIGGERED EXPLOSIVE
	Weapon_Remote_Trigger = 43, // REMOTE TRIGGER
};

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

	bool can_create_shells();

	int32_t get_slot();
	int32_t get_max_mag_ammo();
	int32_t get_bullets_to_fire();
	int32_t get_fire_sound_id();

	float get_bullet_force1();
	float get_bullet_force2();

	vec3 get_muzzle_offset();

	const char* get_type_name();
	const char* get_name();
};

class Weapon
{
public:
	void set_ammo(int32_t v);

	bool is_firing();

	int32_t get_ammo();
	int32_t get_bullets_fired();
	int32_t get_bullets_fired_now();

	float get_last_shot_time();

	WeaponInfo* get_info();
	Character*	get_owner();

	vec3 get_aim_hit_position();

	Transform* get_grip_transform();
	Transform* get_last_muzzle_transform();
	Transform* get_muzzle_transform();
	Transform* get_last_ejector_transform();
};