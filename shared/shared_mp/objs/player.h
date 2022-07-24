#pragma once

#include <shared_mp/objs/net_object.h>

#include <game/transform/transform.h>

#include <game/shared/stances.h>
#include <game/shared/character.h>

class PlayerClient;
class CharacterHandle;

enum PlayerDynamicInfoID : uint32_t
{
	PlayerDynInfo_Transform,
	PlayerDynInfo_Velocity,
	PlayerDynInfo_HeadRotation,
	PlayerDynInfo_Skin,
	PlayerDynInfo_Health,
};

enum PlayerStanceID : uint32_t
{
	PlayerStanceID_Movement,
	PlayerStanceID_Jump,
	PlayerStanceID_Punch,
	PlayerStanceID_BodyStance,
	PlayerStanceID_Aiming,
	PlayerStanceID_Fire,
	PlayerStanceID_Reload,
};

class Player : public NetObject
{
public:

	struct DynamicInfo
	{
		std::string nick;

		Transform transform {};

		vec3 head_rotation {},
			 aim_target {},
			 fire_muzzle {},
			 fire_target {};

		uint32_t skin = 0u,
				 body_stance_id = 0u,
				 arms_stance_id = 0u;

		int32_t weapon_id = 0u,
				firing_weapon_id = 0u;

		float hp = 0.f,
			  max_hp = 0.f,
			  head_interpolation = 0.f;

		bool hip_aim = false,
			 full_aim = false,
			 fire_weapon = false;
	};

	struct MovementInfo
	{
		float angle, right, forward;

		bool aiming;
	};

private:

	DynamicInfo dyn_info {};
	MovementInfo move_info {};

	PlayerClient* client = nullptr;

#ifdef JC_CLIENT
	CharacterHandle* handle = nullptr;

	bool local = false,
		 dispatching_movement = true;
#endif

public:

	static constexpr uint32_t TYPE() { return NetObject_Player; }

	uint32_t get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	Player(PlayerClient* pc, NID nid);

	void verify_exec(const std::function<void(Character*)>& fn);
	void respawn(float hp, float max_hp, bool sync = true);
	void dispatch_movement();
	void set_local() { local = true; }

	bool is_dispatching_movement() const;
	bool is_local() const { return local; }

	Character* get_character() const;
#else
	Player(PlayerClient* pc);

	void verify_exec(auto fn) {}
	void respawn(float hp, float max_hp);
#endif
	~Player();

	bool spawn() override;

	PlayerClient* get_client() const { return client; }

	// dynamic info getters/setters

	void set_hp(float v);
	void set_max_hp(float v);
	void set_transform(const Transform& transform);
	void set_movement_info(float angle, float right, float forward, bool aiming);
	void set_body_stance_id(uint32_t id);
	void set_arms_stance_id(uint32_t id);
	void set_head_rotation(const vec3& v, float interpolation);
	void do_punch();
	void set_weapon_id(int32_t id);
	void set_aim_info(bool hip, bool full, const vec3& target);
	void set_fire_weapon_info(bool fire, int32_t weapon_id = 0, const vec3& muzzle = {}, const vec3& target = {});
	void reload();

	bool is_alive() const { return get_hp() > 0.f; }
	bool is_hip_aiming() const { return dyn_info.hip_aim; }
	bool is_full_aiming() const { return dyn_info.full_aim; }
	bool is_firing() const { return dyn_info.fire_weapon; }

	int32_t get_firing_weapon_id() const { return dyn_info.firing_weapon_id; }
	int32_t get_weapon_id() const { return dyn_info.weapon_id; }

	uint32_t get_body_stance_id() const { return dyn_info.body_stance_id; }
	uint32_t get_arms_stance_id() const { return dyn_info.arms_stance_id; }

	float get_hp() const { return dyn_info.hp / get_max_hp(); }
	float get_max_hp() const { return dyn_info.max_hp; }
	float get_head_interpolation() const { return dyn_info.head_interpolation; }

	const vec3& get_head_rotation() const { return dyn_info.head_rotation; }
	const vec3& get_aim_target() const { return dyn_info.aim_target; }
	const vec3& get_fire_muzzle() const { return dyn_info.fire_muzzle; }
	const vec3& get_fire_target() const { return dyn_info.fire_target; }

	const Transform& get_transform() const { return dyn_info.transform; }

	// basic info getters/setters

	void set_nick(const std::string& v);
	void set_skin(uint32_t v);

	uint32_t get_skin() const;

	const std::string& get_nick() const { return dyn_info.nick; }

	const DynamicInfo& get_dyn_info() const { return dyn_info; }

	const MovementInfo& get_movement_info() const { return move_info; }
};