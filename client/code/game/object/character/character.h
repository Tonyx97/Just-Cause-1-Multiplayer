#pragma once

#include "../alive_object/alive_object.h"
#include "../agent_type/npc_variant.h"

#include "comps/skeleton.h"

#include <game/shared/character.h>

namespace jc::character
{
	static constexpr uint32_t GRENADES_AMMO				= 0x90; // int32_t
	static constexpr uint32_t WEAPON_BELT				= 0xD8; // WeaponBelt
	static constexpr uint32_t SKELETON					= 0xDC; // Skeleton
	static constexpr uint32_t BODY_STANCE_CONTROLLER	= 0x514; // BodyStanceController
	static constexpr uint32_t ARMS_STANCE_CONTROLLER	= 0x568; // ArmsStanceController
	static constexpr uint32_t TRANSFORM					= 0x5D8; // Transform
	static constexpr uint32_t VEHICLE_SEAT				= 0x878; // VehicleSeat*
	static constexpr uint32_t DEATH_TIME				= 0x8E4; // float
	static constexpr uint32_t VELOCITY					= 0x7DC; // vec3
	static constexpr uint32_t AIM_TARGET				= 0x824; // vec3
	static constexpr uint32_t PHYSICAL					= 0x850; // hkCharacterProxy*
	static constexpr uint32_t FLAGS						= 0x884; // uint32_t
	static constexpr uint32_t AIR_TIME					= 0x8EC; // float
	static constexpr uint32_t GRENADE_TIMEOUT			= 0x914; // float
	static constexpr uint32_t GRENADE_TIME				= 0x918; // float
	static constexpr uint32_t HANDLE_ENTRY				= 0x91C; // CharacterHandleEntry
	static constexpr uint32_t INFO						= 0x924; // CharacterInfo*
	static constexpr uint32_t IDLE_STANCE_TIME			= 0xA14; // float
	static constexpr uint32_t IDLE_STANCE_INTERVAL		= 0xA18; // float
	static constexpr uint32_t ROLL_CLAMP				= 0xA34; // float

	namespace fn
	{
		static constexpr uint32_t SET_ANIMATION								= 0x5A1BE0;
		static constexpr uint32_t SET_TRANSFORM								= 0x58F190;
		static constexpr uint32_t SET_NPC_VARIANT							= 0x58CA20;
		static constexpr uint32_t GET_HEAD_BONE_POSITION					= 0x58F610;
		static constexpr uint32_t GET_PELVIS_BONE_POSITION					= 0x58F5A0;
		static constexpr uint32_t GET_STOMACH_BONE_POSITION					= 0x58F530;
		static constexpr uint32_t GET_RANDOM_BONE_POSITION					= 0x58F680;
		static constexpr uint32_t GET_HEAD_BONE_TRANSFORM					= 0x58F6F0;
		static constexpr uint32_t GET_PELVIS_BONE_TRANSFORM					= 0x58F760;
		static constexpr uint32_t DESTROY_SKELETON							= 0x648990;
		static constexpr uint32_t CREATE_SKELETON							= 0x648430;
		static constexpr uint32_t CAN_BE_DESTROYED							= 0x595F10;
		static constexpr uint32_t RESPAWN									= 0x598420;
		static constexpr uint32_t GET_FACING_OBJECT							= 0x596DC0;
		static constexpr uint32_t SET_DRAW_WEAPON							= 0x5A09A0;
		static constexpr uint32_t DRAW_WEAPON_NOW							= 0x59F8E0;
		static constexpr uint32_t RELOAD_CURRENT_WEAPON						= 0x5A0220;
		static constexpr uint32_t FORCE_LAUNCH								= 0x5A34A0;
		static constexpr uint32_t SET_STANCE_ENTER_VEH_RIGHT				= 0x5A1DB0;
		static constexpr uint32_t SET_STANCE_ENTER_VEH_LEFT					= 0x5A1D40;
		static constexpr uint32_t SET_STANCE_ENTER_VEH_SKIP_ANIM			= 0x59F620;
		static constexpr uint32_t SET_STANCE_ENTER_VEH_ROOF					= 0x59F530;
		static constexpr uint32_t SET_STANCE_ENTER_AIRVEH_ROOF				= 0x59F570;
		static constexpr uint32_t SET_STANCE_VEHICLE_ROOF_TO_DRIVER_SEAT	= 0x59F550;
		static constexpr uint32_t SET_STANCE_TO_VEHICLE_ROOF				= 0x59F590;
		static constexpr uint32_t SET_STANCE_EXIT_VEHICLE_FORCED			= 0x59F450;
		static constexpr uint32_t SET_VEHICLE_SEAT							= 0x74DDC0;
		
	}

	namespace g
	{
		static constexpr uint32_t AI_PUNCH_DAMAGE			= 0x5A43F5;
		static constexpr uint32_t PLAYER_PUNCH_DAMAGE		= 0x5A43EC;
		static constexpr uint32_t FLYING_Y_MODIFIER			= 0xAECBB0;

		static constexpr auto DEFAULT_SPAWN_LOCATION		= vec3(-39.87f, 75.81f, -547.04f);
	}

	namespace hook
	{
		void enable(bool apply);
	}
}

class Transform;
class WeaponBelt;
class VehicleSeat;
class hkCharacterProxy;
class BodyStanceController;
class ArmsStanceController;
class NPCVariant;
class CharacterController;
class Weapon;
class Vehicle;

struct CharacterInfo
{
	int is_female;
	float y;
	int	  z;
	float w;

	jc::stl::string name,
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
	void init_model();

public:

	IMPL_OBJECT_TYPE_ID("CCharacter");

	static void SET_GLOBAL_PUNCH_DAMAGE(float v, bool ai = false);
	static void SET_FLYING_Y_MODIFIER(float v);

	void respawn();
	void set_proxy_velocity(const vec3& v);
	void set_added_velocity(const vec3& v);
	void set_grenades_ammo(int32_t v);
	void set_animation(const std::string& name, float speed, bool unk0 = false, bool unk1 = false);
	void set_grenade_timeout(float v);
	void set_walking_anim_set(int32_t walking_anim_id, int32_t skin_id, bool sync = true);
	void set_skin(int32_t id, bool sync = true);
	void set_skin(int32_t id, int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props, bool sync = true);
	void set_npc_variant(NPCVariant* v);
	void set_npc_variant(int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& accessories, bool sync = true);
	void set_flag(uint32_t mask);
	void set_flags(uint32_t mask);
	void remove_flag(uint32_t mask);
	void play_idle_stance();
	void set_roll_clamp_enabled(bool v);
	void dispatch_movement(float angle, float right, float forward, bool aiming);
	void set_body_stance(uint32_t id);
	void set_arms_stance(uint32_t id);
	void setup_punch();
	void force_launch(const vec3& vel, const vec3& dir, float f1, float f2);
	void clear_weapon_belt();
	void set_weapon(uint8_t id, bool is_remote_player = true);
	void set_draw_weapon(int32_t slot);
	void set_draw_weapon(ref<Weapon>& weapon);
	void save_current_weapon();
	void apply_weapon_switch();
	void set_aim_target(const vec3& v);
	void fire_current_weapon(int32_t weapon_id, const vec3& muzzle, const vec3& aim_target);
	void reload_current_weapon();
	void set_stance_enter_vehicle_right(bool skip_anim);
	void set_stance_enter_vehicle_left(bool skip_anim);
	void set_stance_enter_vehicle_no_anim();
	void set_stance_exit_vehicle_forced();

	bool has_flag(uint32_t mask) const;
	bool is_on_ground() const;

	int32_t get_grenades_ammo() const;

	uint32_t get_flags() const;

	float get_grenade_timeout() const;
	float get_grenade_time() const;
	float get_death_time() const;
	float get_roll_clamp() const;
	float get_air_time() const;

	CharacterController* get_controller() const;

	Character* get_facing_object() const;

	WeaponBelt* get_weapon_belt() const;

	Vehicle* get_vehicle() const;

	ref<VehicleSeat> get_vehicle_seat() const;

	Skeleton* get_skeleton() const;

	CharacterInfo* get_info() const;

	BodyStanceController* get_body_stance() const;
	ArmsStanceController* get_arms_stance() const;

	hkCharacterProxy* get_proxy() const;

	vec3 get_velocity();
	vec3 get_added_velocity() const;
	vec3 get_aim_target() const;
	vec3 get_bone_position(BoneID index) const;
	vec3 get_head_bone_position();
	vec3 get_pelvis_bone_position();
	vec3 get_stomach_bone_position();
	vec3 get_random_bone_position();

	Transform get_head_bone_transform();
	Transform get_pelvis_bone_transform();
};