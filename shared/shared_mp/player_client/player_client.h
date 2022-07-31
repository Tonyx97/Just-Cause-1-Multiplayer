#pragma once

class Player;

class PlayerClient
{
private:

	Player* player = nullptr;

	bool initialized = false,
		 joined = false;

#ifdef JC_SERVER
	ENetPeer* peer = nullptr;

	bool timed_out = false;

	template <typename... A>
	inline void send_impl(uint8_t channel, const std::vector<uint8_t>& data, uint32_t flags = ENET_PACKET_FLAG_RELIABLE)
	{
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

	void set_initialized(bool v);
	void set_joined(bool v);
	void set_nick(const std::string& v);

	bool is_initialized() const { return initialized; }
	bool is_joined() const { return joined; }

	NID get_nid() const;

	Player* get_player() const { return player; }

	const std::string& get_nick() const;

#ifdef JC_SERVER
	/**
	* syncs everything we need for this player, we also sync stuff from this
	* player to other players so they know about the existence and data of the player
	*/
	void startup_sync();

	template <typename T>
	inline void send_reliable(const T& packet)
	{
		send_impl(T::CHANNEL, packet.serialize());
	}

	inline void send(uint8_t channel, const PacketHolder& p)
	{
		enet::send_packet(peer, channel, p);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_reliable(PacketID id, const A&... args)
	{
		std::vector<uint8_t> data;

		enet::serialize_params(data, id, args...);

		send_impl(channel, data);
}

	void set_timed_out() { timed_out = true; }

	bool is_timed_out() const { return timed_out; }

	ENetPeer* get_peer() const { return peer; }
#endif
};

#define AS_PC(pc)							BITCAST(PlayerClient*, pc)
#define CREATE_PLAYER_CLIENT(nid_or_peer)	JC_ALLOC(PlayerClient, nid_or_peer)
#define DESTROY_PLAYER_CLIENT(pc)			JC_FREE(pc)