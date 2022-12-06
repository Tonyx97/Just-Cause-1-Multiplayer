#pragma once

#include <shared_mp/object_lists.h>

#include "settings.h"
#include "config.h"
#include "users_db.h"

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

	UsersDB users_db {};

	ENetHost* sv = nullptr;

	netcp::tcp_server* tcp_server = nullptr;

	void send_broadcast_impl(const Packet& p, PlayerClient* ignore_pc = nullptr);
	void send_broadcast_joined_impl(const Packet& p, PlayerClient* ignore_pc = nullptr);

public:

	~Net();

	bool init();
	void tick();
	void refresh_net_object_sync();
	void sync_net_objects(bool force = false);

	void send_broadcast(const Packet& p) { send_broadcast(nullptr, p); }
	void send_broadcast(PlayerClient* ignore_pc, const Packet& p) { send_broadcast_impl(p, ignore_pc); }
	void send_broadcast_joined(const Packet& p) { send_broadcast_joined(nullptr, p); }
	void send_broadcast_joined(PlayerClient* ignore_pc, const Packet& p) { send_broadcast_impl(p, ignore_pc); }
	void sync_default_files(netcp::tcp_server_client* cl);
	void sync_resource(const std::string& rsrc_name);

	WorldRg* get_rg() const { return world_rg; }

	ENetHost* get_host() const { return sv; }

	Settings& get_settings() { return settings; }

	Config& get_config() { return config; }

	UsersDB& get_users() { return users_db; }

	// callbacks

	void on_client_tcp_connected(netcp::tcp_server_client* ci);
	void on_client_tcp_disconnected(netcp::tcp_server_client* ci);
	void on_client_tcp_message(netcp::client_interface* ci, const netcp::packet_header* header, serialization_ctx& data);
};

inline Net* g_net = nullptr;