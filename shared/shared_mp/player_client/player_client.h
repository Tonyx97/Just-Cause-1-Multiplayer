#pragma once

class Player;

class PlayerClient
{
private:

	Player* player = nullptr;

	bool initialized = false,
		 joined = false;

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

	void set_initialized(bool v);
	void set_joined(bool v);
	void set_nick(const std::string& v);

	bool is_initialized() const { return initialized; }
	bool is_joined() const { return joined; }

	NID get_nid() const;

	Player* get_player() const { return player; }

	const std::string& get_nick() const;

#ifdef JC_SERVER
	/**
	* syncs everything we need for this player, we also sync stuff from this
	* player to other players so they know about the existence and data of the player
	*/
	void startup_sync();

	void send(const Packet& p, bool create = false)
	{
		if (create)
			p.create();

		enet::send_packet(peer, p);
	}

	void set_timed_out() { timed_out = true; }

	bool is_timed_out() const { return timed_out; }
	bool compare_address(const ENetAddress& peer);

	ENetPeer* get_peer() const { return peer; }

	ENetAddress* get_address() const { return &peer->address; }
#endif
};

#define AS_PC(pc)							BITCAST(PlayerClient*, pc)
#define CREATE_PLAYER_CLIENT(nid_or_peer)	JC_ALLOC(PlayerClient, nid_or_peer)
#define DESTROY_PLAYER_CLIENT(pc)			JC_FREE(pc)