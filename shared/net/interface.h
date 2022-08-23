#pragma once

#include "channels.h"
#include "packets.h"
#include "serializer.h"

struct PacketHolder
{
	ENetPacket* packet = nullptr;

	PacketHolder(const std::vector<uint8_t>& data, uint32_t flags = ENET_PACKET_FLAG_RELIABLE)
	{
		packet = enet_packet_create(data.data(), data.size(), flags);
	}

	~PacketHolder()
	{
		if (packet && packet->referenceCount == 0)
			enet_packet_destroy(packet);
	}
};

#include <shared_mp/packets/all.h>

class PlayerClient;

namespace enet
{
	static constexpr uint32_t GAME_PORT		= 19971;
	static constexpr uint32_t MAX_PLAYERS	= 128;
	static constexpr uint32_t TICKS			= 62;
	static constexpr uint32_t TICKS_MS		= 16;
#ifdef JC_REL
	static constexpr uint32_t PEER_TIMEOUT = 10 * 1000;
#else
	static constexpr uint32_t PEER_TIMEOUT = 10000 * 1000;
#endif

	enum PacketResult
	{
		PacketRes_Ok,
		PacketRes_NotFound,
		PacketRes_NotUsable,
		PacketRes_BadArgs,
		PacketRes_NotSupported,
		PacketRes_NotAllowed,
	};

#ifdef JC_CLIENT
	ENetPeer* GET_CLIENT_PEER();
#endif

	ENetHost* GET_HOST();

	/**
	 * Class used to read a packet
	 * this class could be improved by allowing the server to access the serialized data directly
	 * instead of deserializing and serializing again to send it to the clients, for now we will keep
	 * it like this
	 */
	class Packet
	{
	private:

		mutable std::vector<uint8_t> data;

		std::vector<uint8_t> initial_data;

#ifdef JC_SERVER
		PlayerClient*  pc	   = nullptr;
#endif
		ENetPeer*	   peer    = nullptr;
		ENetPacket*	   packet  = nullptr;
		mutable size_t offset  = 0u;
		float		   time	   = 0.f;
		uint8_t		   id	   = 0u;
		uint8_t		   channel = 0u;
		bool		   as_view = false,
					   destroyed = false;

	public:

		Packet(const ENetEvent& e, bool view = false)
			: peer(e.peer)
			, packet(e.packet)
			, channel(e.channelID)
			, as_view(view)
			, time(util::time::get_time())
		{
			::check(!view, "Not supported yet");

			// create data vector from the enet packet

			initial_data = data = { e.packet->data, e.packet->data + e.packet->dataLength };

			// first get the id of the packet and erase it from the vector

			id = deserialize_int<uint8_t>(data);

#ifdef JC_SERVER
			pc = BITCAST(PlayerClient*, e.peer->data);
#endif
		}

		~Packet()
		{
			if (destroyed)
				return;

			if (!as_view)
			{
				check(packet, "Packet was already destroyed or it's invalid");

				enet_packet_destroy(packet);

				destroyed = true;
				packet = nullptr;
			}
		}

		// avoid copies

		Packet(const Packet&) = delete;

		// force copy elision

		Packet(Packet&& other)
		{
			*this = std::move(other);
		}

		// implement move operator

		Packet& operator=(Packet&& other)
		{
#ifdef JC_SERVER
			pc = other.pc;
#endif

			initial_data = data = other.data;
			peer = other.peer;
			packet = other.packet;
			offset = other.offset;
			id = other.id;
			time = other.time;
			channel = other.channel;
			as_view = other.as_view;

			other.destroyed = true;
			other.packet = nullptr;

			return *this;
		}

		template <typename T>
		void skip() const
		{
			offset += sizeof(T);
		}

		bool is_empty() const { return data.empty(); }

		int32_t get_flags() const { return packet->flags; }

		float get_time() const { return time; }

		ENetPeer* get_peer() const { return peer; }

		std::vector<uint8_t> get_initial_data() const { return initial_data; }

#ifdef JC_SERVER
		PlayerClient* get_pc() const { return pc; }
#endif

		uint8_t get_id() const { return id; }
		uint8_t get_channel() const { return channel; }

		uint8_t get_u8() const { return deserialize_int<uint8_t>(data); }
		int8_t get_i8() const { return deserialize_int<uint8_t>(data); }

		uint16_t get_u16() const { return deserialize_int<uint16_t>(data); }
		int16_t get_i16() const { return deserialize_int<int16_t>(data); }

		uint32_t get_u32() const { return deserialize_int<uint32_t>(data); }
		int32_t get_i32() const { return deserialize_int<int32_t>(data); }

		uint64_t get_u64() const { return deserialize_int<uint64_t>(data); }
		int64_t get_i64() const { return deserialize_int<int64_t>(data); }

		template <typename T>
		T get_integral() const { return deserialize_int<T>(data); }

		bool get_bool() const { return deserialize_bool(data); }

		float get_float() const { return deserialize_float(data); }

		template <typename T>
		T get() const { return T().deserialize(data); }

		template <typename T>
		T get_raw() const { return deserialize_general_data<T>(data); }

		std::string get_str() const { return deserialize_string(data); }

		std::pair<NID, NetObjectType> get_nid_and_type() const
		{
			const auto _nid = get_u32();
			const auto _type = get_u8();

			return { _nid, _type };
		}

		template <typename T = NetObject>
		T* get_net_object() const
		{
			if (const auto net_obj = deserialize_net_object(data))
			{
				if constexpr (std::is_same_v<T, NetObject>)
					return net_obj;
				else return net_obj->cast_safe<T>();
			}

			return nullptr;
		}

		template <typename T>
		std::vector<T> get_vector() const
		{
			return deserialize_vector<T>(data);
		}
	};

	using channel_dispatch_t = std::function<PacketResult(const Packet&)>;

	void init();
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

	inline bool send_packet(ENetPeer* peer, uint8_t channel, const PacketHolder& p)
	{
		check(peer && p.packet, "Invalid peer or packet");

		return enet_peer_send(peer, channel, p.packet) == 0;
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