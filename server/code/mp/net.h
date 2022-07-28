#pragma once

#include <shared_mp/object_lists.h>

#include "settings.h"

class Net : public ObjectLists
{
private:
	Settings settings {};

	ENetHost* sv = nullptr;

	inline void send_broadcast_reliable(uint8_t channel, const PacketHolder& p, PlayerClient* ignore_pc = nullptr)
	{
		for_each_player_client([&](NID, PlayerClient* pc)
		{
			if (pc != ignore_pc)
				pc->send(channel, p);
		});
	}

	inline void send_broadcast_joined_reliable(uint8_t channel, const PacketHolder& p, PlayerClient* ignore_pc = nullptr)
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

	template <typename T>
	inline void send_broadcast_reliable(const T& data)
	{
		send_broadcast_reliable(T::CHANNEL, data.serialize_as_packet(), nullptr);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_reliable(PacketID id, const A&... args)
	{
		send_broadcast_reliable<channel>(nullptr, id, args...);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_reliable(PlayerClient* ignore_pc, PacketID id, const A&... args)
	{
		vec<uint8_t> data;

		enet::serialize_params(data, id, args...);

		send_broadcast_reliable(channel, PacketHolder(data), ignore_pc);
	}

	template <typename T>
	inline void send_broadcast_joined_reliable(const T& data)
	{
		send_broadcast_joined_reliable(T::CHANNEL, data.serialize_as_packet(), nullptr);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_joined_reliable(PlayerClient* ignore_pc, PacketID id, const A&... args)
	{
		vec<uint8_t> data;

		enet::serialize_params(data, id, args...);

		send_broadcast_joined_reliable(channel, PacketHolder(data), ignore_pc);
	}

	ENetHost* get_host() const { return sv; }

	Settings& get_settings() { return settings; }
};

inline Net* g_net = nullptr;