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
	void setup_channels();
	void tick();

	/*
	* updates the objects while we don't receive any data such as the player stance animation
	* where we want to set the last one without receiving the stance all the time
	*/
	void update_objects();

	void set_net_stats(int v) { net_stat = v; }

	inline void send_packet(uint32_t channel, const std::vector<uint8_t>& data, uint32_t flags = ENET_PACKET_FLAG_RELIABLE)
	{
		enet::send_packet(enet::GET_CLIENT_PEER(), data.data(), data.size(), flags, channel);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_reliable(PacketID id, const A&... args)
	{
		std::vector<uint8_t> data;

		enet::serialize_params(data, id, args...);

		send_packet(channel, data);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_unreliable(PacketID id, const A&... args)
	{
		std::vector<uint8_t> data;

		enet::serialize_params(data, id, args...);

		send_packet(channel, data, ENET_PACKET_FLAG_UNSEQUENCED);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_unreliable(const std::vector<uint8_t>& data)
	{
		send_packet(channel, data, ENET_PACKET_FLAG_UNSEQUENCED);
	}

	template <typename T>
	inline void send_reliable(const T& packet)
	{
		send_packet(T::CHANNEL, packet.serialize());
	}

	template <typename T>
	inline void send_unreliable(const T& packet)
	{
		send_packet(T::CHANNEL, packet.serialize(), ENET_PACKET_FLAG_UNSEQUENCED);
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