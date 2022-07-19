#pragma once

#include <shared_mp/objs/net_object.h>

#include <game/transform/transform.h>

#include <game/shared/stances.h>

class PlayerClient;
class CharacterHandle;

struct PlayerStaticInfo
{
	std::string nick;

	uint32_t skin = 0u;
};

enum PlayerDynamicInfoID : uint32_t
{
	PlayerDynInfo_Transform,
	PlayerDynInfo_Velocity,
	PlayerDynInfo_HeadRotation,
	PlayerDynInfo_Skin
};

enum PlayerStanceID : uint32_t
{
	PlayerStanceID_Movement,
	PlayerStanceID_Jump,
	PlayerStanceID_Punch,
	PlayerStanceID_BodyStance,
};

class Player : public NetObject
{
public:

	struct DynamicInfo
	{
		Transform transform {};

		vec3 head_rotation {};

		uint32_t body_stance_id = 0u,
				 arms_stance_id = 0u;

		float hp = 0.f;
	};

	struct MovementInfo
	{
		float angle, right, forward;

		bool aiming;
	};

private:

	PlayerStaticInfo static_info{};
	DynamicInfo dyn_info {};
	MovementInfo move_info {};

	PlayerClient* client = nullptr;

#ifdef JC_CLIENT
	CharacterHandle* handle = nullptr;

	bool local = false;
#endif

public:

	bool skip_engine_stances = true;

	static constexpr uint32_t TYPE() { return NetObject_Player; }

	uint32_t get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	Player(PlayerClient* pc, NID nid);

	void verify_exec(const std::function<void(Character*)>& fn);
	void set_local() { local = true; }
#else
	Player(PlayerClient* pc);

	void verify_exec(auto fn) {}
#endif
	~Player();

	bool spawn() override;

	PlayerClient* get_client() const { return client; }

	// dynamic info getters/setters

	void set_hp(float v);
	void set_transform(const Transform& transform);
	void set_movement_info(float angle, float right, float forward, bool aiming);
	void set_body_stance_id(uint32_t id);
	void set_arms_stance_id(uint32_t id);
	void set_head_rotation(const vec3& v);
	void do_punch();

	bool is_alive() const { return get_hp() > 0.f; }

	uint32_t get_body_stance_id() const { return dyn_info.body_stance_id; }
	uint32_t get_arms_stance_id() const { return dyn_info.arms_stance_id; }

	float get_hp() const { return dyn_info.hp; }

	const vec3& get_head_rotation() const { return dyn_info.head_rotation; }

	const Transform& get_transform() const { return dyn_info.transform; }

	// static info getters/setters

	void set_nick(const std::string& v);
	void set_skin(uint32_t v);

#ifdef JC_CLIENT
	Character* get_character() const;

	bool is_local() const { return local; }
#endif

	bool must_skip_engine_stances() const;

	uint32_t get_skin() const;

	const std::string& get_nick() const { return static_info.nick; }

	const DynamicInfo& get_dyn_info() const { return dyn_info; }

	const PlayerStaticInfo& get_static_info() const { return static_info; }

	const MovementInfo& get_movement_info() const { return move_info; }
};