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

		uint32_t body_stances_id[4] = { 0u },
				 arms_stances_id[4] = { 0u };

		int body_stances_count = 0,
			arms_stances_count = 0;

		void clear()
		{
			body_stances_count = arms_stances_count = 0;

			memset(body_stances_id, 0, sizeof(body_stances_id));
			memset(arms_stances_id, 0, sizeof(arms_stances_id));
		}
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

	void clear_tick_info() { tick_info.clear(); }
	void set_tick_info(const TickInfo& v);

	bool spawn() override;

	const PlayerStaticInfo& get_static_info() const { return static_info; }

	PlayerClient* get_client() const { return client; }

	// tick info getters/setters

	void set_transform(const Transform& transform);
	void set_body_stance_id(uint32_t id);
	void set_arms_stance_id(uint32_t id);

	uint32_t get_body_stance_id(int index) const { return tick_info.body_stances_id[index]; }
	uint32_t get_arms_stance_id(int index) const { return tick_info.arms_stances_id[index]; }

	Transform get_transform() const { return tick_info.transform; }

	// static info getters/setters

	void set_nick(const std::string& v);
	void set_skin(uint32_t v);

	uint32_t get_skin() const;

	const std::string& get_nick() const { return static_info.nick; }

	const TickInfo& get_tick_info() const { return tick_info; }
};