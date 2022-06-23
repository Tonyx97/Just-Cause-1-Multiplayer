#pragma once

#ifndef JC_LOADER

class Player;

class PlayerClient
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

#ifdef JC_CLIENT
	PlayerClient(NID nid);
#else
	PlayerClient(ENetPeer* peer);
#endif
	~PlayerClient();

	void set_nick(const std::string& v);
	void set_ready();

	bool is_ready() const { return ready; }

	NID get_nid() const;

	Player* get_player() const { return player; }

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
#define DESERIALIZE_NID_AND_TYPE(p)			const auto nid = p.get_int<NID>(); \
											const auto type = p.get_int<uint32_t>()
#endif