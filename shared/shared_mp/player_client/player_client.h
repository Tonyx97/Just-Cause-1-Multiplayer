#pragma once

#ifdef JC_SERVER
#include <thread_system/stl/queue.h>
#endif

class Player;
class Resource;

namespace netcp
{
	class tcp_server_client;
}

DEFINE_ENUM(PlayerClientRole, uint64_t)
{
	PlayerClientRole_Normal			= (1 << 0),
	PlayerClientRole_Moderator		= (1 << 1),
	PlayerClientRole_Admin			= (1 << 2),
	PlayerClientRole_Owner			= (1 << 3),
};

class PlayerClient
{
private:

	Player* player = nullptr;

	bool initialized = false,
		 joined = false;

#ifdef JC_SERVER
	jc::thread_safe::queue<std::string> resources_to_sync;

	mutable std::mutex tcp_mtx;

	netcp::tcp_server_client* tcp = nullptr;

	ENetPeer* peer = nullptr;

	PlayerClientRole roles = PlayerClientRole_Normal;

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
	void sync_pending_resources();
	void set_tcp(netcp::tcp_server_client* v);
	void add_role(PlayerClientRole v);
	void set_role(PlayerClientRole v);
	void remove_role(PlayerClientRole v);
	void startup_sync();
	void add_resource_to_sync(const std::string& rsrc_name) { resources_to_sync.push(rsrc_name); }

	/**
	* syncs this player with the rest of the server
	*/
	void sync_broadcast();
	void sync_player(Player* target_player, bool create);
	void sync_entity(NetObject* target_entity, bool create);

	void send(const Packet& p, bool create = false)
	{
		if (create)
			p.create();

		enet::send_packet(peer, p);
	}

	void set_timed_out() { timed_out = true; }

	bool is_timed_out() const { return timed_out; }
	bool compare_address(const ENetAddress& peer);

	PlayerClientRole get_roles() const { return roles; }

	ENetPeer* get_peer() const { return peer; }

	ENetAddress* get_address() const { return &peer->address; }

	netcp::tcp_server_client* get_tcp() const { return tcp; }

	std::string get_ip() const;
#endif
};

#define AS_PC(pc)							BITCAST(PlayerClient*, pc)
#define CREATE_PLAYER_CLIENT(nid_or_peer)	JC_ALLOC(PlayerClient, nid_or_peer)
#define DESTROY_PLAYER_CLIENT(pc)			JC_FREE(pc)