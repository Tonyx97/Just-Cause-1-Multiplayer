#pragma once

#ifndef AVOID_INTERFACE_RECURSION
#include <enet/interface.h>
#endif

class Player;

class PlayerClient
{
private:

	ENetPeer* peer = nullptr;

	Player* player = nullptr;

	std::string nick;

	bool ready = false,
		 timed_out = false;

public:

	PlayerClient(ENetPeer* peer);
	~PlayerClient();

	void set_nick(const std::string& v);
	void set_ready() { ready = true; }
	void set_timed_out() { timed_out = true; }

	bool is_ready() const { return ready; }
	bool is_timed_out() const { return timed_out; }

	ENetPeer* get_peer() const { return peer; }

	const std::string& get_nick() const { return nick; }
};

#define AS_PC(pc)					(BITCAST(PlayerClient*, pc))
#define CREATE_PLAYER_CLIENT(peer)	JC_ALLOC(PlayerClient, peer)
#define DESTROY_PLAYER_CLIENT(pc)	JC_FREE(pc)