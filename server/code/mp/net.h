#pragma once

#include <shared_mp/object_lists.h>

#include "settings.h"

class Net : public ObjectLists
{
private:
	Settings settings {};

	ENetHost* sv = nullptr;

	inline void send_broadcast(uint8_t channel, const PacketHolder& p, PlayerClient* ignore_pc = nullptr)
	{
		for_each_player_client([&](NID, PlayerClient* pc)
		{
			if (pc != ignore_pc)
				pc->send(channel, p);
		});
	}

	inline void send_broadcast_joined(uint8_t channel, const PacketHolder& p, PlayerClient* ignore_pc = nullptr)
	{
		for_each_joined_player_client([&](NID, PlayerClient* pc)
		{
			if (pc != ignore_pc)
				pc->send(channel, p);
		});
	}

public:
	bool init();
	void destroy();
	void setup_channels();
	void tick();
	void refresh_net_object_sync();
	void sync_net_objects();

	template <typename T>
	inline void send_broadcast_reliable(const T& data)
	{
		send_broadcast(T::CHANNEL, data.serialize_as_packet(), nullptr);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_reliable(PacketID id, const A&... args)
	{
		send_broadcast_reliable<channel>(nullptr, id, args...);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_reliable(PlayerClient* ignore_pc, PacketID id, const A&... args)
	{
		send_broadcast<channel>(ENET_PACKET_FLAG_RELIABLE, ignore_pc, id, args...);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_unreliable(PacketID id, const A&... args)
	{
		send_broadcast_unreliable<channel>(nullptr, id, args...);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_unreliable(PlayerClient* ignore_pc, PacketID id, const A&... args)
	{
		send_broadcast<channel>(ENET_PACKET_FLAG_UNSEQUENCED, ignore_pc, id, args...);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_unreliable(PlayerClient* ignore_pc, const std::vector<uint8_t>& data)
	{
		send_broadcast<channel>(ENET_PACKET_FLAG_UNSEQUENCED, ignore_pc, data);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast(int32_t flags, PlayerClient* ignore_pc, PacketID id, const A&... args)
	{
		std::vector<uint8_t> data;

		enet::serialize_params(data, id, args...);

		send_broadcast(channel, PacketHolder(data, flags), ignore_pc);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast(int32_t flags, PlayerClient* ignore_pc, const std::vector<uint8_t>& data)
	{
		send_broadcast(channel, PacketHolder(data, flags), ignore_pc);
	}

	template <typename T>
	inline void send_broadcast_joined(const T& data)
	{
		send_broadcast_joined(T::CHANNEL, data.serialize_as_packet(), nullptr);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_joined_reliable(PlayerClient* ignore_pc, PacketID id, const A&... args)
	{
		std::vector<uint8_t> data;

		enet::serialize_params(data, id, args...);

		send_broadcast_joined(channel, PacketHolder(data), ignore_pc);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_joined_unreliable(PlayerClient* ignore_pc, PacketID id, const A&... args)
	{
		std::vector<uint8_t> data;

		enet::serialize_params(data, id, args...);

		send_broadcast_joined(channel, PacketHolder(data, ENET_PACKET_FLAG_UNSEQUENCED), ignore_pc);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_joined_reliable(PacketID id, const A&... args)
	{
		std::vector<uint8_t> data;

		enet::serialize_params(data, id, args...);

		send_broadcast_joined(channel, PacketHolder(data), nullptr);
	}

	ENetHost* get_host() const { return sv; }

	Settings& get_settings() { return settings; }
};

inline Net* g_net = nullptr;