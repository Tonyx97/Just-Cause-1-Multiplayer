#pragma once

#include <shared_mp/object_lists.h>

class Net : public ObjectLists
{
private:
	ENetHost* sv = nullptr;

public:
	bool init();
	void destroy();
	void setup_channels();
	void tick();
	void send_global_packets();

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_reliable(PlayerClient* ignore_pc, uint32_t id, const A&... args)
	{
		enet::PacketW p(id);

		p.add(args...);
		p.ready();

		for_each_player_client([&](NID, PlayerClient* pc)
		{
			if (pc != ignore_pc)
				pc->send<channel>(p);
		});
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_reliable(uint32_t id, const A&... args)
	{
		send_broadcast_reliable<channel>(nullptr, id, args...);
	}

	ENetHost* get_host() const { return sv; };
};

inline Net* g_net = nullptr;