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
		 ready = false,
		 timed_out = false;

	void disconnect();

public:
	bool init(const std::string& ip, const std::string& nick);
	void destroy();
	void add_local(NID nid);
	void set_ready();
	void setup_channels();
	void tick();
	void set_net_stats(int v) { net_stat = v; }

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_reliable(uint32_t id, const A&... args)
	{
		vec<uint8_t> data;

		enet::serialize(data, 1u, id);
		enet::serialize_params(data, 1u, args...);
		enet::send_packet(enet::GET_CLIENT_PEER(), data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE, channel);
	}

	bool is_ready() const { return ready; }

	PlayerClient* get_local() const { return local; }
	Player* get_localplayer() const { return local->get_player(); }

	ENetHost* get_host() const { return client; }
	ENetPeer* get_peer() const { return peer; }

	const std::string& get_nick() const { return nick; }
};

inline Net* g_net = nullptr;