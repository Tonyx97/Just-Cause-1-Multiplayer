#pragma once

#include <enet/interface.h>

class Player;

class PlayerClient
{
private:

	Player* player = nullptr;

	std::string nick;

	NID nid = INVALID_NID;

	bool ready = false;

public:

	PlayerClient();
	~PlayerClient();

	void set_nick(const std::string& v);
	void set_ready() { ready = true; }

	bool is_ready() const { return ready; }

	NID get_nid() const { return nid; }

	const std::string& get_nick() const { return nick; }
};

#define AS_PC(pc)					(BITCAST(PlayerClient*, pc))
#define CREATE_PLAYER_CLIENT(peer)	JC_ALLOC(PlayerClient)
#define DESTROY_PLAYER_CLIENT(pc)	JC_FREE(pc)