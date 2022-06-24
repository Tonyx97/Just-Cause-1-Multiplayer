#pragma once

#ifndef JC_LOADER

#include <shared_mp/objs/net_object.h>

class PlayerClient;

struct PlayerStaticInfo
{
	std::string nick;
};

class Player : public NetObject
{
private:

	PlayerStaticInfo static_info;

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

	void set_nick(const std::string& v);

	PlayerClient* get_client() const { return client; }

	const std::string& get_nick() const { return static_info.nick; }
};
#endif