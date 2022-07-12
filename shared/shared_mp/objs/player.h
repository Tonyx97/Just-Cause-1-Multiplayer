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

	struct TickInfo : public PacketBase
	{
		Transform transform;

		uint32_t body_stance_id = 0u,
				 arms_stance_id = 0u;
	};

private:

	CharacterHandle* handle = nullptr;

	PlayerStaticInfo static_info {};

	PlayerClient* client = nullptr;

	TickInfo tick_info {};

public:

	static constexpr uint32_t TYPE() { return NetObject_Player; }

	uint32_t get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	Player(PlayerClient* pc, NID nid);

	void verify_exec(const std::function<void(Character*)>& fn);
#else
	Player(PlayerClient* pc);

	void verify_exec(auto fn) {}
#endif
	~Player();

	bool spawn() override;

	const PlayerStaticInfo& get_static_info() const { return static_info; }

	PlayerClient* get_client() const { return client; }

	// tick info getters/setters

	void set_transform(const Transform& transform);
	void set_body_stance_id(uint32_t id);
	void set_arms_stance_id(uint32_t id);

	uint32_t get_body_stance_id() const { return tick_info.body_stance_id; }
	uint32_t get_arms_stance_id() const { return tick_info.arms_stance_id; }

	Transform get_transform() const { return tick_info.transform; }

	// static info getters/setters

	void set_nick(const std::string& v);
	void set_skin(uint32_t v);

	uint32_t get_skin() const;

	const std::string& get_nick() const { return static_info.nick; }

	const TickInfo& get_tick_info() const { return tick_info; }
};