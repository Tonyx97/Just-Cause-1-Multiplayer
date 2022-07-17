#pragma once

#include <shared_mp/objs/net_object.h>

#include <game/transform/transform.h>

class PlayerClient;
class CharacterHandle;

struct PlayerStaticInfo
{
	std::string nick;

	uint32_t skin = 0u;
};

class Player : public NetObject
{
public:

	struct TickInfo
	{
		Transform transform {};

		float hp;
	};

	struct DynamicInfo
	{
		uint32_t body_stance_id = 0u,
				 arms_stance_id = 0u;
	};

	struct MovementInfo
	{
		float angle, right, forward;

		bool aiming;
	};

private:

	CharacterHandle* handle = nullptr;

	PlayerStaticInfo static_info {};

	PlayerClient* client = nullptr;

	TickInfo tick_info {};
	DynamicInfo dyn_info {};
	MovementInfo move_info {};

#ifdef JC_CLIENT
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

	// tick info getters/setters

	void set_hp(float v);
	void set_transform(const Transform& transform);
	void set_movement_info(float angle, float right, float forward, bool aiming);

	bool is_alive() const;

	float get_hp() const;

	const Transform& get_transform() const { return tick_info.transform; }

	// dynamic info getters/setters

	void set_body_stance_id(uint32_t id);
	void set_arms_stance_id(uint32_t id);

	uint32_t get_body_stance_id() const { return dyn_info.body_stance_id; }
	uint32_t get_arms_stance_id() const { return dyn_info.arms_stance_id; }

	// static info getters/setters

	void set_tick_info(const TickInfo& v);
	void set_nick(const std::string& v);
	void set_skin(uint32_t v);

#ifdef JC_CLIENT
	Character* get_character() const;

	bool is_local() const { return local; }
#endif

	bool must_skip_engine_stances() const;

	uint32_t get_skin() const;

	const std::string& get_nick() const { return static_info.nick; }

	const TickInfo& get_tick_info() const { return tick_info; }

	const PlayerStaticInfo& get_static_info() const { return static_info; }

	const MovementInfo& get_movement_info() const { return move_info; }
};