#pragma once

#include <shared_mp/object_lists.h>

#include <thread_system/stl/vector.h>

#define DEBUG_PACKET 1

class PlayerClient;

namespace netcp
{
	class tcp_client;
	class client_interface;

	struct packet_header;
}

struct TcpContext
{
	std::atomic_bool password_ack = false;
	std::atomic_bool password_valid = false;
	std::atomic_bool default_server_files_received = false;
	std::atomic_bool startup_all_resources_synced = false;
	
	struct ResourceContext
	{
		jc::thread_safe::vector<std::string> downloading_resources;

		size_t total_size = 0u;
	} rsrc;
};

class Net : public ObjectLists
{
private:

	ENetHost* client = nullptr;
	ENetPeer* peer	 = nullptr;

	PlayerClient* local = nullptr;

	std::string nick {};

	netcp::tcp_client* tcp = nullptr;

	TcpContext tcp_ctx {};

#if DEBUG_PACKET
	std::map<PacketID, uint64_t> packets_sent_count;

	int net_stat = 4;
#else
	int net_stat = 0;
#endif

	bool connected = false,
		 timed_out = false,
		 initialized = false,
		 joined = false,
		 game_ready_to_load = false,
		 game_loaded = false;

	void disconnect();

public:

	static constexpr auto DEFAULT_SERVER_FILES_PATH() { return "default_server_files\\"; }

	bool init(const std::string& ip, const std::string& pw, const std::string& nick);
	void pre_destroy();
	void destroy();
	void add_local(NID nid);
	void set_initialized(bool v);
	void set_joined(bool v);
	void set_game_ready_to_load();
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

#if DEBUG_PACKET
		++packets_sent_count[p.get_id()];
#endif

		enet::send_packet(peer, p);
	}

	bool is_tcp_connected() const;
	bool is_initialized() const { return initialized; }
	bool is_joined() const { return joined; }
	bool is_game_ready_to_load() const { return game_ready_to_load; }

	int get_net_stat() const { return net_stat; }

	PlayerClient* get_local() const { return local; }
	Player* get_localplayer() const;

	ENetHost* get_host() const { return client; }
	ENetPeer* get_peer() const { return peer; }

	const std::string& get_nick() const { return nick; }

#if DEBUG_PACKET
	template <typename Fn>
	void for_each_packet_sent(const Fn& fn)
	{
		for (const auto& [pid, count] : packets_sent_count)
			fn(pid, count);
	}
#endif

	// callbacks

	void on_tcp_message(netcp::client_interface* ci, const netcp::packet_header* header, serialization_ctx& data);
};

inline Net* g_net = nullptr;