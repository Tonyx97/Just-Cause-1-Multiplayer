#pragma once

#include "serializer.h"
#include "packets.h"
#include "channels.h"

class PlayerClient;

namespace enet
{
	static constexpr uint32_t GAME_PORT	   = 19971;
	static constexpr uint32_t MAX_PLAYERS  = 128;
	static constexpr uint32_t PEER_TIMEOUT = 120 * 1000;

	enum PacketResult
	{
		PacketRes_Ok,
		PacketRes_NotFound,
		PacketRes_NotUsable,
	};

#ifdef JC_CLIENT
	ENetPeer* GET_CLIENT_PEER();
	ENetHost* GET_CLIENT_HOST();
#else
	ENetHost* GET_HOST();
#endif

	inline void send_packet_broadcast(ENetHost* host, const void* data, size_t size, ENetPacketFlag flags = ENET_PACKET_FLAG_RELIABLE, uint8_t channel = ChannelID_Generic)
	{
		if (!host)
			return;

		const auto packet = enet_packet_create(data, size, flags);

		check(packet, "Net packet could not be created");

		enet_host_broadcast(host, channel, packet);
	}

	inline bool send_packet(ENetPeer* peer, const void* data, size_t size, ENetPacketFlag flags = ENET_PACKET_FLAG_RELIABLE, uint8_t channel = ChannelID_Generic)
	{
		if (!peer)
			return false;

		const auto packet = enet_packet_create(data, size, flags);

		check(packet, "Net packet could not be created");

		return enet_peer_send(peer, channel, packet) == 0;
	}

	inline bool send_packet(ENetPeer* peer, const std::string& data, ENetPacketFlag flags = ENET_PACKET_FLAG_RELIABLE, uint8_t channel = ChannelID_Generic)
	{
		return send_packet(peer, data.data(), data.length() + 1, flags, channel);
	}

	template <typename T>
	inline bool send_packet(ENetPeer* peer, const T& data, ENetPacketFlag flags = ENET_PACKET_FLAG_RELIABLE, uint8_t channel = ChannelID_Generic)
	{
		return send_packet(peer, &data, sizeof(data), flags, channel);
	}

#ifdef JC_CLIENT
	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_reliable(uint32_t id, const A&... args)
	{
		vec<uint8_t> data;

		serialize(data, id);
		serialize_params(data, args...);

		send_packet(GET_CLIENT_PEER(), data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE, channel);
	}
#else
	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_reliable(ENetPeer* peer, uint32_t id, const A&... args)
	{
		vec<uint8_t> data;

		serialize(data, id);
		serialize_params(data, args...);

		send_packet(peer, data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE, channel);
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_reliable(uint32_t id, const A&... args)
	{
		vec<uint8_t> data;

		serialize(data, id);
		serialize_params(data, args...);

		send_packet_broadcast(GET_HOST(), data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE, channel);
}
#endif

	/**
	 * Class used to read a packet
	 */
	class PacketR
	{
	private:
#ifdef JC_SERVER
		PlayerClient*  pc	   = nullptr;
#endif
		ENetPeer*	   peer    = nullptr;
		ENetPacket*	   packet  = nullptr;
		uint8_t*	   data	   = nullptr;
		mutable size_t offset  = 0u;
		size_t		   size	   = 0u;
		uint32_t	   id	   = 0u;
		uint8_t		   channel = 0u;
		bool		   as_view = false;

	public:
		PacketR(const ENetEvent& e, bool view = false)
			: peer(e.peer)
			, packet(e.packet)
			, data(e.packet->data)
			, size(e.packet->dataLength)
			, channel(e.channelID)
			, as_view(view)
		{
			check(!view, "Not supported yet");

			id = get_int<uint32_t>();

#ifdef JC_SERVER
			pc = BITCAST(PlayerClient*, e.peer->data);
#endif
		}

		~PacketR()
		{
			if (!as_view)
				enet_packet_destroy(packet);
		}

		ENetPeer* get_peer() const { return peer; }

#ifdef JC_SERVER
		PlayerClient* get_player_client_owner() const { return pc; }
#endif

		uint32_t get_id() const { return id; }
		uint8_t get_channel() const { return channel; }

		template <typename T>
		T get_int() const
		{
			return *BITCAST(T*, data + std::exchange(offset, offset + sizeof(T)));
		}

		float get_float() const
		{
			return get_int<float>();
		}

		NetObject* get_net_obj() const;

		template <typename T>
		T get_str() const
		{
			const auto len = *BITCAST(size_t*, data + std::exchange(offset, offset + sizeof(size_t)));

			T out;

			out.resize(len);

			memcpy(out.data(), data + std::exchange(offset, offset + len), len);

			return out;
		}
	};

	/**
	 * Class used to write and send a packet
	 */
	class PacketW
	{
	private:
		vec<uint8_t> data;

		ENetPacketFlag flags;

		uint32_t id = 0u;

	public:
		PacketW(uint32_t id, ENetPacketFlag flags = ENET_PACKET_FLAG_RELIABLE)
			: id(id)
			, flags(flags)
		{
			add(id);
		}

		~PacketW() {}

		uint32_t get_id() const { return id; }

		template <typename... A>
		void add(const A&... args)
		{
			serialize_params(data, args...);
		}

#ifdef JC_CLIENT
		/**
		 * This function is used by the client project where
		 * the peer is always the same, the client -> server one
		 */
		void send(uint8_t channel = ChannelID_Generic)
		{
			send_packet(GET_CLIENT_PEER(), data.data(), data.size(), flags, channel);
		}
#endif

		/**
		 * This function is used by the client and the server
		 */
		void send(ENetPeer* peer, uint8_t channel = ChannelID_Generic)
		{
			send_packet(peer, data.data(), data.size(), flags, channel);
		}

#ifdef JC_SERVER
		/**
		 * This function is used by the server only to send a packet
		 * to all clients
		 */
		void send_broadcast(uint8_t channel = ChannelID_Generic)
		{
			send_packet_broadcast(GET_HOST(), data.data(), data.size(), flags, channel);
		}
#endif
	};

	using channel_dispatch_t = std::function<PacketResult(const PacketR&)>;

	void init();
	void add_channel_dispatcher(uint8_t id, const channel_dispatch_t& fn);
	void call_channel_dispatcher(const ENetEvent& id);

	template <typename Fn>
	inline void dispatch_packets(const Fn& fn, uint32_t timeout = 0)
	{
		ENetEvent e;

#ifdef JC_CLIENT
		ENetHost* host = GET_CLIENT_HOST();
#else
		ENetHost* host = GET_HOST();
#endif

		while (enet_host_service(host, &e, timeout) > 0)
			fn(e);
	}

	template <typename Fn>
	inline void dispatch_packet(const Fn& fn, uint32_t timeout = 0)
	{
		ENetEvent e;

#ifdef JC_CLIENT
		ENetHost* host = GET_CLIENT_HOST();
#else
		ENetHost* host = GET_HOST();
#endif

		if (enet_host_service(host, &e, timeout) > 0)
			fn(e);
	}

	template <typename Fn>
	inline bool wait_until_packet(uint32_t id, uint8_t channel, const Fn& fn, uint32_t timeout = 5000)
	{
		bool received = false;

		auto a = std::chrono::steady_clock::now();

		while (!received && std::chrono::duration_cast<decltype(0ms)>(std::chrono::steady_clock::now() - a).count() < timeout)
		{
			dispatch_packet([&](ENetEvent& e)
			{
				if (PacketR p(e); !received && p.get_id() == id && p.get_channel() == channel)
				{
					fn(p);

					received = true;
				}
			}, timeout);
		}

		return received;
	}
}