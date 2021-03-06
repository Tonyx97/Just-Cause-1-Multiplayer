#pragma once

#include <shared_mp/objs/net_object.h>

#include <game/transform/transform.h>

#include <game/shared/stances.h>
#include <game/shared/character.h>
#include <game/shared/npc_variant.h>

class PlayerClient;
class CharacterHandle;

enum PlayerDynamicInfoID : uint8_t
{
	PlayerDynInfo_Transform,
	PlayerDynInfo_Velocity,
	PlayerDynInfo_HeadRotation,
	PlayerDynInfo_Skin,
	PlayerDynInfo_Health,
	PlayerDynInfo_NPCVariant,
};

enum PlayerStanceID : uint8_t
{
	PlayerStanceID_Movement,
	PlayerStanceID_MovementAngle,
	PlayerStanceID_Jump,
	PlayerStanceID_Punch,
	PlayerStanceID_BodyStance,
	PlayerStanceID_Aiming,
	PlayerStanceID_Fire,
	PlayerStanceID_Reload,
	PlayerStanceID_ForceLaunch,
};

class Player : public NetObject
{
public:

	struct DynamicInfo
	{
		std::string nick;

		quat rotation;

		vec3 position {},
			 velocity {},
			 head_rotation{},
			 aim_target {};

		uint32_t skin = 0u,
				 body_stance_id = 0u,
				 arms_stance_id = 0u;

		int32_t weapon_id = 0u,
				firing_weapon_id = 0u;

		float hp = 0.f,
			  max_hp = 0.f,
			  head_interpolation = 0.f;

		bool hip_aim = false,
			 full_aim = false;
	};

	struct MovementInfo
	{
		float angle = 0.f,
			  right = 0.f,
			  forward = 0.f;

		bool sync_angle_next_tick = false,
			 aiming = false;
	};

	struct SkinInfo
	{
		std::vector<VariantPropInfo> props;

		int32_t cloth_skin = 0,
				head_skin = 0,
				cloth_color = 0;
	};

private:

	DynamicInfo dyn_info {};
	MovementInfo move_info{};
	SkinInfo skin_info {};

	PlayerClient* client = nullptr;

#ifdef JC_CLIENT
	CharacterHandle* handle = nullptr;

	bool local = false,
		 dispatching_movement = true,
		 correcting_position = false;
#endif

public:

	static constexpr NetObjectType TYPE() { return NetObject_Player; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	Player(PlayerClient* pc, NID nid);

	void verify_exec(const std::function<void(Character*)>& fn);
	void respawn(float hp, float max_hp, bool sync = true);
	void dispatch_movement();
	void correct_position();
	void set_local() { local = true; }

	bool is_dispatching_movement() const;
	bool is_local() const { return local; }

	Character* get_character() const;

	CharacterHandle* get_character_handle() const;
#else
	Player(PlayerClient* pc);

	void verify_exec(auto fn) {}
	void respawn(float hp, float max_hp);
#endif
	~Player();

	bool spawn() override;

	vec3 get_position() const override { return dyn_info.position; }
	quat get_rotation() const override { return dyn_info.rotation; }

	PlayerClient* get_client() const { return client; }

	// dynamic info getters/setters

	void set_nick(const std::string& v);
	void set_skin(uint32_t v);
	void set_hp(float v);
	void set_max_hp(float v);
	void set_transform(const vec3& position, const quat& rotation);
	void set_velocity(const vec3& v);
	void set_movement_angle(float angle, bool send_angle_only_next_tick);
	void set_movement_info(float angle, float right, float forward, bool aiming);
	void set_body_stance_id(uint32_t id);
	void set_arms_stance_id(uint32_t id);
	void set_head_rotation(const vec3& v, float interpolation);
	void do_punch();
	void force_launch(const vec3& vel, const vec3& dir, float f1, float f2);
	void set_weapon_id(int32_t id);
	void set_aim_info(bool hip, bool full, const vec3& target);
	void fire_current_weapon(int32_t weapon_id = 0, const vec3& muzzle = {}, const vec3& target = {});
	void reload();
	void set_skin_info(int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props);

	bool is_alive() const { return get_hp() > 0.f; }
	bool is_hip_aiming() const { return dyn_info.hip_aim; }
	bool is_full_aiming() const { return dyn_info.full_aim; }
	bool should_sync_angle_only() const { return move_info.sync_angle_next_tick; }

	int32_t get_firing_weapon_id() const { return dyn_info.firing_weapon_id; }
	int32_t get_weapon_id() const { return dyn_info.weapon_id; }

	uint32_t get_skin() const { return dyn_info.skin; }
	uint32_t get_body_stance_id() const { return dyn_info.body_stance_id; }
	uint32_t get_arms_stance_id() const { return dyn_info.arms_stance_id; }

	float get_hp() const { return dyn_info.hp / get_max_hp(); }
	float get_max_hp() const { return dyn_info.max_hp; }
	float get_head_interpolation() const { return dyn_info.head_interpolation; }

	const vec3& get_head_rotation() const { return dyn_info.head_rotation; }
	const vec3& get_aim_target() const { return dyn_info.aim_target; }

	const std::string& get_nick() const { return dyn_info.nick; }

	const vec3& get_velocity() const { return dyn_info.velocity; }

	// basic info getters/setters

	const DynamicInfo& get_dyn_info() const { return dyn_info; }

	const MovementInfo& get_movement_info() const { return move_info; }
};