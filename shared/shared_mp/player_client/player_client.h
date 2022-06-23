#pragma once

#ifndef JC_LOADER

#include <shared_mp/objs/net_object.h>

class Player;

class PlayerClient : public NetObject
{
private:

	Player* player = nullptr;

	std::string nick;

	bool ready = false;

#ifdef JC_SERVER
	ENetPeer* peer = nullptr;

	bool timed_out = false;
#endif

public:

	static constexpr uint32_t TYPE() { return NetObject_Player; }

	uint32_t get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	PlayerClient(NID nid);
#else
	PlayerClient(ENetPeer* peer);
#endif
	~PlayerClient();

	void set_nick(const std::string& v);
	void set_ready() { ready = true; }

	bool is_ready() const { return ready; }

	const std::string& get_nick() const { return nick; }

#ifdef JC_SERVER
	void set_timed_out() { timed_out = true; }

	bool is_timed_out() const { return timed_out; }

	ENetPeer* get_peer() const { return peer; }
#endif
};

#define AS_PC(pc)							BITCAST(PlayerClient*, pc)
#define CREATE_PLAYER_CLIENT(nid_or_peer)	JC_ALLOC(PlayerClient, nid_or_peer)
#define DESTROY_PLAYER_CLIENT(pc)			JC_FREE(pc)
#endif