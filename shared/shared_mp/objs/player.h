#pragma once

#include <shared_mp/objs/net_object.h>

#include <game/shared/stances.h>
#include <game/shared/character.h>
#include <game/shared/npc_variant.h>
#include <game/shared/vehicle_seat.h>

class PlayerClient;
class VehicleNetObject;
class CharacterHandle;
class UIMapIcon;

DEFINE_ENUM(PlayerDynamicInfoID, uint8_t)
{
	PlayerDynInfo_HeadRotation,
	PlayerDynInfo_Skin,
	PlayerDynInfo_WalkingSetAndSkin,
	PlayerDynInfo_NPCVariant,
};

DEFINE_ENUM(PlayerStanceID, uint8_t)
{
	PlayerStanceID_Movement,
	PlayerStanceID_MovementAngle,
	PlayerStanceID_Jump,
	PlayerStanceID_Punch,
	PlayerStanceID_BodyStance,
	PlayerStanceID_Aiming,
	PlayerStanceID_Fire,
	PlayerStanceID_FireMultiple,
	PlayerStanceID_Reload,
	PlayerStanceID_ForceLaunch,
};

class Player : public NetObject
{
public:

	struct DynamicInfo
	{
#ifdef JC_CLIENT
		std::mt19937_64 multiple_bullet_mt;

		bool use_multiple_bullet = false;
#endif

		std::string nick;

		vec3 velocity {},
			 head_rotation{},
			 muzzle_position {},
			 bullet_dir {},
			 aim_target {};

		int32_t skin = 0,
				walking_set = 0;

		int32_t	body_stance_id = 0,
				arms_stance_id = 0;

		int32_t weapon_id = 0,
				firing_weapon_id = 0;

		float head_interpolation = 0.f;

		bool hip_aim = false,
			 full_aim = false;
	};

	struct MovementInfo
	{
		float angle = 0.f,
			  right = 0.f,
			  forward = 0.f;

		bool sync_angle_next_tick = false,
			 force_sync = false,
			 aiming = false;
	};

	struct SkinInfo
	{
		std::vector<VariantPropInfo> props;

		int32_t cloth_skin = -1,
				head_skin = -1,
				cloth_color = -1;
	};

private:

	DynamicInfo dyn_info {};
	MovementInfo move_info {};
	SkinInfo skin_info {};

	VehicleNetObject* vehicle = nullptr;

	uint8_t vehicle_seat = VehicleSeat_None;

	bool tag_enabled = true;

	PlayerClient* client = nullptr;

#ifdef JC_CLIENT
	UIMapIcon* blip = nullptr;

	CharacterHandle* handle = nullptr;

	bool local = false,
		 dispatching_movement = true,
		 correcting_position = false;
#endif

	void destroy_object();

public:

	static constexpr NetObjectType TYPE() { return NetObject_Player; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	Player(PlayerClient* pc, NID nid);

	Character* get_character() const;

	class ObjectBase* get_object_base() const override;

	void verify_exec(const std::function<void(Character*)>& fn);
	void dispatch_movement();
	void correct_position();
	void update_blip();
	void set_multiple_rand_seed(uint16_t v);
	void respawn_character();
	void set_local() { local = true; }

	bool is_dispatching_movement() const;
	bool is_local() const { return local; }

	uint16_t should_use_multiple_rand_seed() const { return dyn_info.use_multiple_bullet; }

	CharacterHandle* get_character_handle() const;

	UIMapIcon* get_blip() const { return blip; }

	vec3 generate_bullet_rand_spread();
#else
	Player(PlayerClient* pc);

	void verify_exec(auto fn) {}

	/**
	* sends all ownerships of the client
	*/
	void send_ownerships();
#endif

	~Player();

	void on_spawn() override;
	void on_despawn() override;
	void on_sync() override {}
	void on_net_var_change(NetObjectVarType var_type) override;
	void serialize_derived(const Packet* p) override {}
	void deserialize_derived(const Packet* p) override {}
	void serialize_derived_create(const Packet* p) override {}
	void deserialize_derived_create(const Packet* p) override {}

	PlayerClient* get_client() const { return client; }

	// dynamic info getters/setters

	void respawn(const vec3& position, float rotation, int32_t skin, float hp, float max_hp);
	void set_tag_enabled(bool v);
	void set_nick(const std::string& v);
	void set_skin(int32_t v);
	void set_skin(int32_t v, int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props);
	void set_walking_set_and_skin(int32_t walking_set_id, int32_t skin_id);
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
	void set_bullet_direction(const vec3& muzzle, const vec3& dir);
	void fire_current_weapon(int32_t weapon_id = 0, const vec3& muzzle = {}, const vec3& dir = {});
	void reload();
	void set_skin_info(int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props);
	void set_vehicle(uint8_t seat_type, VehicleNetObject* v);

	bool is_tag_enabled() const { return tag_enabled; }
	bool is_alive() const { return get_hp() > 0.f; }
	bool is_hip_aiming() const { return dyn_info.hip_aim; }
	bool is_full_aiming() const { return dyn_info.full_aim; }
	bool should_sync_angle_only() const { return move_info.sync_angle_next_tick; }
	bool should_force_sync_movement_info() const { return move_info.force_sync; }

	uint8_t get_vehicle_seat() const { return vehicle_seat; }

	int32_t get_firing_weapon_id() const { return dyn_info.firing_weapon_id; }
	int32_t get_weapon_id() const { return dyn_info.weapon_id; }

	uint32_t get_skin() const { return dyn_info.skin; }
	uint32_t get_body_stance_id() const { return dyn_info.body_stance_id; }
	uint32_t get_arms_stance_id() const { return dyn_info.arms_stance_id; }

	float get_head_interpolation() const { return dyn_info.head_interpolation; }

	VehicleNetObject* get_vehicle() const { return vehicle; }

	const vec3& get_head_rotation() const { return dyn_info.head_rotation; }
	const vec3& get_aim_target() const { return dyn_info.aim_target; }
	const vec3& get_muzzle_position() const { return dyn_info.muzzle_position; }
	const vec3& get_bullet_direction() const { return dyn_info.bullet_dir; }

	const std::string& get_nick() const { return dyn_info.nick; }

	const vec3& get_velocity() const { return dyn_info.velocity; }

	// basic info getters/setters

	const DynamicInfo& get_dyn_info() const { return dyn_info; }

	const MovementInfo& get_movement_info() const { return move_info; }

	const SkinInfo& get_skin_info() const { return skin_info; }
};