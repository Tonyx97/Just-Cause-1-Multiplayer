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

	int net_stat = 0;

	bool connected = false,
		 timed_out = false;

	void disconnect();

public:
	bool init(const std::string& ip, const std::string& nick);
	void destroy();
	void setup_channels();
	void tick();
	void set_net_stats(int v) { net_stat = v; }

	PlayerClient* get_local() const { return local; }

	ENetHost* get_host() const { return client; }
	ENetPeer* get_peer() const { return peer; }

	const std::string& get_nick() const { return nick; }
};

inline Net* g_net = nullptr;