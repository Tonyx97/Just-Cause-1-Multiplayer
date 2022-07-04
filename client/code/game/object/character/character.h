#pragma once

#include "../alive_object/alive_object.h"

#include "comps/skeleton.h"

namespace jc::character
{
	static constexpr uint32_t GRENADES_AMMO		 = 0x90; // int32_t
	static constexpr uint32_t WEAPON_BELT		 = 0xD8; // WeaponBelt
	static constexpr uint32_t SKELETON			 = 0xDC; // Skeleton
	static constexpr uint32_t STANCE_CONTROLLER	 = 0x514; // StanceController
	static constexpr uint32_t TRANSFORM			 = 0x5D8; // Transform
	static constexpr uint32_t VEHICLE_CONTROLLER = 0x878; // VehicleController*
	static constexpr uint32_t DEATH_TIME		 = 0x8E4; // float
	static constexpr uint32_t VELOCITY			 = 0x7DC; // vec3
	static constexpr uint32_t FLAGS				 = 0x884; // uint32_t
	static constexpr uint32_t GRENADE_TIMEOUT	 = 0x914; // float
	static constexpr uint32_t GRENADE_TIME		 = 0x918; // float
	static constexpr uint32_t INFO				 = 0x924; // CharacterInfo*

	namespace fn
	{
		static constexpr uint32_t SET_ANIMATION				= 0x5A1BE0;
		static constexpr uint32_t SET_TRANSFORM				= 0x58F190;
		static constexpr uint32_t SET_NPC_VARIANT			= 0x58CA20;
		static constexpr uint32_t GET_HEAD_BONE_POSITION	= 0x58F610;
		static constexpr uint32_t GET_PELVIS_BONE_POSITION	= 0x58F5A0;
		static constexpr uint32_t GET_STOMACH_BONE_POSITION = 0x58F530;
		static constexpr uint32_t GET_RANDOM_BONE_POSITION	= 0x58F680;
		static constexpr uint32_t GET_HEAD_BONE_TRANSFORM	= 0x58F6F0;
		static constexpr uint32_t GET_PELVIS_BONE_TRANSFORM = 0x58F760;
		static constexpr uint32_t DESTROY_SKELETON			= 0x648990;
		static constexpr uint32_t CREATE_SKELETON			= 0x648430;
		static constexpr uint32_t CAN_BE_DESTROYED			= 0x595F10;
		
	}

	namespace g
	{
		static constexpr uint32_t AI_PUNCH_DAMAGE	  = 0x5A43F5;
		static constexpr uint32_t PLAYER_PUNCH_DAMAGE = 0x5A43EC;
	}

	namespace hook
	{
		using set_animation_internal_t = prototype<void(__thiscall*)(void*, int*, ptr, std::string*, bool, float), 0x7915E0, util::hash::JENKINS("CharSetAnim")>;

		static constexpr uint32_t SET_ANIMATION_INTERNAL = 0x7915E0;

		void apply();
		void undo();
	}
}

class Transform;
class WeaponBelt;
class VehicleController;
class hkCharacterProxy;
class StanceController;
class NPCVariant;

struct CharacterInfo
{
	int is_female;
	float y;
	int	  z;
	float w;

	std::string name,
		model,
		eyelid,
		skeleton,
		character_as,
		upper_body_as,
		base_character_as,
		base_upper_body_as;
};

class Character : public AliveObject
{
private:
	void rebuild_skeleton();

public:
	static constexpr auto CLASS_NAME() { return "CCharacter"; }
	static constexpr auto CLASS_ID() { return util::hash::JENKINS(CLASS_NAME()); }

	static void SET_GLOBAL_PUNCH_DAMAGE(float v, bool ai = false);

	void set_grenades_ammo(int32_t v);
	void set_animation(const std::string& name, float speed, bool unk0 = false, bool unk1 = false);
	void set_grenade_timeout(float v);
	void set_model(uint32_t id);
	void set_npc_variant(NPCVariant* v);
	void set_flag(uint32_t mask);
	void remove_flag(uint32_t mask);

	bool has_flag(uint32_t mask);

	int32_t get_grenades_ammo();

	uint32_t get_flags();

	float get_grenade_timeout();
	float get_grenade_time();
	float get_death_time();

	WeaponBelt* get_weapon_belt() const;

	VehicleController* get_vehicle_controller();

	Skeleton* get_skeleton() const;

	CharacterInfo* get_info() const;

	StanceController* get_stance_controller() const;

	vec3 get_velocity();
	vec3 get_bone_position(BoneID index) const;
	vec3 get_head_bone_position();
	vec3 get_pelvis_bone_position();
	vec3 get_stomach_bone_position();
	vec3 get_random_bone_position();

	Transform get_head_bone_transform();
	Transform get_pelvis_bone_transform();
};