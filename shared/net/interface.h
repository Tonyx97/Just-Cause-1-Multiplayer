#pragma once

#define ENET_FEATURE_ADDRESS_MAPPING

#include <enet.h>

#include <shared_mp/objs/all.h>

#include <serializer/serializer.h>

#include <net/obj_serializer.h>

#include "channels.h"
#include "packets.h"
#include "packet.h"

class PlayerClient;

namespace enet
{
	static constexpr uint32_t MAX_PLAYERS	= 128;
	static constexpr uint32_t TICKS			= 62;
	static constexpr uint32_t TICKS_MS		= 16;
#ifdef JC_REL
	static constexpr uint32_t PEER_TIMEOUT = 10 * 1000;
#else
	static constexpr uint32_t PEER_TIMEOUT = 10000 * 1000;
#endif

#ifdef JC_CLIENT
	ENetPeer* GET_CLIENT_PEER();
#endif

	ENetHost* GET_HOST();

	using channel_dispatch_t = std::function<PacketResult(const Packet&)>;

	void init();
	void setup_channels();
	void add_channel_dispatcher(uint8_t id, const channel_dispatch_t& fn);
	void call_channel_dispatcher(const ENetEvent& id);
	void set_fake_lag(int value);
	void process_lagged_packets();

	int get_fake_lag();

	inline bool send_packet(ENetPeer* peer, const void* data, size_t size, uint32_t flags = ENET_PACKET_FLAG_RELIABLE, uint8_t channel = ChannelID_Generic)
	{
		check(peer && data && size > 0, "Invalid peer or packet");

		const auto packet = enet_packet_create(data, size, flags);

		check(packet, "Net packet could not be created");

		return enet_peer_send(peer, channel, packet) == 0;
	}

	inline bool send_packet(ENetPeer* peer, const Packet& p)
	{
		const auto enet_packet = p.get_packet();

		check(peer, "Invalid peer");
		check(enet_packet, "Invalid packet");

		return enet_peer_send(peer, p.get_channel(), enet_packet) == 0;
	}

	template <typename Fn>
	inline void dispatch_packets(const Fn& fn, uint32_t timeout = 0)
	{
		ENetEvent e;

		while (enet_host_service(GET_HOST(), &e, timeout) > 0)
			fn(e);

#ifdef JC_CLIENT
		enet::process_lagged_packets();
#endif
	}

	template <typename Fn>
	inline void dispatch_packet(const Fn& fn, uint32_t timeout = 0)
	{
		ENetEvent e;

		if (enet_host_service(GET_HOST(), &e, timeout) > 0)
			fn(e);
	}

	template <typename Fn>
	inline bool wait_until_packet(uint8_t id, uint8_t channel, const Fn& fn, uint32_t timeout = 5000)
	{
		bool received = false;

		auto a = std::chrono::steady_clock::now();

		while (!received && std::chrono::duration_cast<decltype(0ms)>(std::chrono::steady_clock::now() - a).count() < timeout)
		{
			dispatch_packet([&](ENetEvent& e)
			{
				if (Packet p(e); !received && p.get_id() == id && p.get_channel() == channel)
				{
					fn(p);

					received = true;
				}
			}, timeout);
		}

		return received;
	}
}