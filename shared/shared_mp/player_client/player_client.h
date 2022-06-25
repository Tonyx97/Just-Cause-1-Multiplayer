#pragma once

class Player;

class PlayerClient
{
private:

	Player* player = nullptr;

	bool ready = false;

#ifdef JC_SERVER
	ENetPeer* peer = nullptr;

	bool timed_out = false;

	template <typename... A>
	inline void send_impl(uint8_t channel, uint32_t id, uint32_t flags, const A&... args)
	{
		vec<uint8_t> data;

		enet::serialize(data, 1u, id);
		enet::serialize_params(data, 1u, args...);
		enet::send_packet(peer, data.data(), data.size(), flags, channel);
	}
#endif

public:

#ifdef JC_CLIENT
	PlayerClient(NID nid);
#else
	PlayerClient(ENetPeer* peer);
#endif
	~PlayerClient();

	void set_nick(const std::string& v);
	void set_ready();

	bool is_ready() const { return ready; }

	NID get_nid() const;

	Player* get_player() const { return player; }

	const std::string& get_nick() const;

#ifdef JC_SERVER
	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_reliable(uint32_t id, const A&... args)
	{
		send_impl(channel, id, ENET_PACKET_FLAG_RELIABLE, args...);
	}

	template <uint8_t channel = ChannelID_Generic>
	inline void send(const enet::PacketW& p)
	{
		enet::send_packet(peer, p.get_packet(), channel);
	}

	void set_timed_out() { timed_out = true; }

	bool is_timed_out() const { return timed_out; }

	ENetPeer* get_peer() const { return peer; }
#endif
};

#define AS_PC(pc)							BITCAST(PlayerClient*, pc)
#define CREATE_PLAYER_CLIENT(nid_or_peer)	JC_ALLOC(PlayerClient, nid_or_peer)
#define DESTROY_PLAYER_CLIENT(pc)			JC_FREE(pc)
#define DESERIALIZE_NID_AND_TYPE(p)			const auto nid = p.get_uint(); \
											const auto type = p.get_uint()