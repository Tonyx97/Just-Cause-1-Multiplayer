#pragma once

#include <shared_mp/object_lists.h>

class PlayerClient;

class Net : public ObjectLists
{
private:

	ENetHost* client = nullptr;
	ENetPeer* peer	 = nullptr;

	PlayerClient* local = nullptr;

	std::string nick {};

#ifdef JC_DBG
	int net_stat = 3;
#else
	int net_stat = 0;
#endif

	bool connected = false,
		 timed_out = false,
		 initialized = false,
		 joined = false;

	void disconnect();

public:
	bool init(const std::string& ip, const std::string& nick);
	void pre_destroy();
	void destroy();
	void add_local(NID nid);
	void set_initialized(bool v);
	void set_joined(bool v);
	void tick();

	/*
	* updates the objects while we don't receive any data such as the player stance animation
	* where we want to set the last one without receiving the stance all the time
	*/
	void update_objects();

	void set_net_stats(int v) { net_stat = v; }

	void send(const Packet& p)
	{
		p.create();

		enet::send_packet(peer, p);
	}

	bool is_initialized() const { return initialized; }
	bool is_joined() const { return joined; }

	int get_net_stat() const { return net_stat; }

	PlayerClient* get_local() const { return local; }
	Player* get_localplayer() const;

	ENetHost* get_host() const { return client; }
	ENetPeer* get_peer() const { return peer; }

	const std::string& get_nick() const { return nick; }
};

inline Net* g_net = nullptr;