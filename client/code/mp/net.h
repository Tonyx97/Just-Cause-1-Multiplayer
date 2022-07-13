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
		 timed_out = false,
		 initialized = false,
		 joined = false;

	void disconnect();

public:
	bool init(const std::string& ip, const std::string& nick);
	void destroy();
	void add_local(NID nid);
	void set_initialized(bool v);
	void set_joined(bool v);
	void setup_channels();
	void tick();

	/*
	* updates the objects while we don't receive any data such as the player stance animation
	* where we want to set the last one without receiving the stance all the time
	*/
	void update_objects();

	void set_net_stats(int v) { net_stat = v; }

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_reliable(uint32_t id, const A&... args)
	{
		vec<uint8_t> data;

		enet::serialize(data, 1u, id);
		enet::serialize_params(data, 1u, args...);
		enet::send_packet(enet::GET_CLIENT_PEER(), data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE, channel);
	}

	bool is_initialized() const { return initialized; }
	bool is_joined() const { return joined; }

	PlayerClient* get_local() const { return local; }
	Player* get_localplayer() const { return local ? local->get_player() : nullptr; }

	ENetHost* get_host() const { return client; }
	ENetPeer* get_peer() const { return peer; }

	const std::string& get_nick() const { return nick; }
};

inline Net* g_net = nullptr;