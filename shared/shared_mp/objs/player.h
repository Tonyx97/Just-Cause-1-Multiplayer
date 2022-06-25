#pragma once

#include <shared_mp/objs/net_object.h>

class PlayerClient;

struct PlayerStaticInfo
{
	std::string nick;

	uint32_t skin = 0u;
};

class Player : public NetObject
{
private:

	PlayerStaticInfo static_info {};

	PlayerClient* client = nullptr;

public:

	static constexpr uint32_t TYPE() { return NetObject_Player; }

	uint32_t get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	Player(PlayerClient* pc, NID nid);
#else
	Player(PlayerClient* pc);
#endif
	~Player();

	const PlayerStaticInfo& get_static_info() const { return static_info; }

	PlayerClient* get_client() const { return client; }

	// static info getters/setters

	void set_nick(const std::string& v);
	void set_skin(uint32_t v);

	uint32_t get_skin() const;

	const std::string& get_nick() const { return static_info.nick; }
};