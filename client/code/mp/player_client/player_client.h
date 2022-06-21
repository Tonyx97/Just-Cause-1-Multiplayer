#pragma once

#include <enet/net_obj.h>

class Player;

class PlayerClient : public NetObject
{
private:

	Player* player = nullptr;

	std::string nick;

	bool ready = false;

public:

	PlayerClient(NID nid);
	~PlayerClient();

	static constexpr uint32_t TYPE() { return NetObject_Player; }

	uint32_t get_type() const override { return TYPE(); }

	void set_nick(const std::string& v);
	void set_ready() { ready = true; }

	bool is_ready() const { return ready; }

	const std::string& get_nick() const { return nick; }
};

#define AS_PC(pc)					BITCAST(PlayerClient*, pc)
#define CREATE_PLAYER_CLIENT(nid)	JC_ALLOC(PlayerClient, nid)
#define DESTROY_PLAYER_CLIENT(pc)	JC_FREE(pc)