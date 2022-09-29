#pragma once

#include <shared_mp/object_lists.h>

#include "settings.h"
#include "config.h"

class WorldRg;

namespace netcp
{
	class tcp_server;
	class tcp_server_client;
	class client_interface;

	struct packet_header;
}

class Net : public ObjectLists
{
private:

	WorldRg* world_rg = nullptr;

	Settings settings {};

	Config config {};

	ENetHost* sv = nullptr;

	netcp::tcp_server* tcp_server = nullptr;

	void send_broadcast_impl(const Packet& p, PlayerClient* ignore_pc = nullptr);
	void send_broadcast_joined_impl(const Packet& p, PlayerClient* ignore_pc = nullptr);

public:

	bool init();
	void destroy();
	void tick();
	void refresh_net_object_sync();
	void sync_net_objects(bool force = false);

	void send_broadcast(const Packet& p) { send_broadcast(nullptr, p); }
	void send_broadcast(PlayerClient* ignore_pc, const Packet& p) { send_broadcast_impl(p, ignore_pc); }
	void send_broadcast_joined(const Packet& p) { send_broadcast_joined(nullptr, p); }
	void send_broadcast_joined(PlayerClient* ignore_pc, const Packet& p) { send_broadcast_impl(p, ignore_pc); }
	void sync_default_files(netcp::tcp_server_client* cl);

	WorldRg* get_rg() const { return world_rg; }

	ENetHost* get_host() const { return sv; }

	Settings& get_settings() { return settings; }
	Config& get_config() { return config; }

	// callbacks

	void on_client_tcp_message(netcp::client_interface* ci, const netcp::packet_header* header, serialization_ctx& data);
};

inline Net* g_net = nullptr;