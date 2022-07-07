#pragma once

#include "channels.h"
#include "packets.h"
#include "serializer.h"

class PlayerClient;

namespace enet
{
	static constexpr uint32_t GAME_PORT	   = 19971;
	static constexpr uint32_t MAX_PLAYERS  = 128;
	static constexpr uint32_t PEER_TIMEOUT = 120 * 1000 * 1000;

	enum PacketResult
	{
		PacketRes_Ok,
		PacketRes_NotFound,
		PacketRes_NotUsable,
		PacketRes_BadArgs
	};

#ifdef JC_CLIENT
	ENetPeer* GET_CLIENT_PEER();
#endif

	ENetHost* GET_HOST();

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

		NetObject* get_net_object_impl() const;

		void get_data(void* out, size_t out_size) const;

	public:
		PacketR(const ENetEvent& e, bool view = false)
			: peer(e.peer)
			, packet(e.packet)
			, data(e.packet->data)
			, size(e.packet->dataLength)
			, channel(e.channelID)
			, as_view(view)
		{
			::check(!view, "Not supported yet");

			id = get_uint();

#ifdef JC_SERVER
			pc = BITCAST(PlayerClient*, e.peer->data);
#endif
		}

		~PacketR()
		{
			if (!as_view)
				enet_packet_destroy(packet);
		}

		template <typename T>
		void skip() const
		{
			offset += sizeof(T);
		}

		ENetPeer* get_peer() const { return peer; }

#ifdef JC_SERVER
		PlayerClient* get_pc() const { return pc; }
#endif

		uint32_t get_id() const { return id; }
		uint8_t get_channel() const { return channel; }

		template <typename T = int>
		T get_int() const
		{
			return *BITCAST(T*, data + std::exchange(offset, offset + sizeof(T)));
		}

		template <typename T = uint32_t>
		T get_uint() const
		{
			return get_int<T>();
		}

		bool get_bool() const
		{
			return get_int<bool>();
		}

		float get_float() const
		{
			return get_int<float>();
		}

		template <typename T>
		T get_struct() const
		{
			T v;

			get_data(&v, sizeof(v));

			return std::move(v);
		}

		template <typename T = NetObject>
		T* get_net_object() const
		{
			if (const auto net_obj = get_net_object_impl())
			{
				if constexpr (std::is_same_v<T, NetObject>)
					return net_obj;
				else return net_obj->cast_safe<T>();
			}

			return nullptr;
		}

		template <typename T = std::string>
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

		ENetPacket* packet = nullptr;
		
		uint32_t flags = 0u,
				 id = 0u;

	public:
		PacketW(uint32_t id, uint32_t flags = ENET_PACKET_FLAG_RELIABLE) : id(id), flags(flags)
		{
			add(id);
		}

		~PacketW()
		{
			if (packet->referenceCount == 0)
				enet_packet_destroy(packet);
		}

		uint32_t get_id() const { return id; }

		template <typename... A>
		auto add_begin(const A&... args)
		{
			return serialize_params(data, 0u, args...);
		}

		template <typename... A>
		auto add(const A&... args)
		{
			return serialize_params(data, 1u, args...);
		}

		template <typename T>
		void replace(auto& it, const T& v)
		{
			auto new_it = data.erase(it, it + sizeof(v));

			const auto data_ptr = BITCAST(uint8_t*, &v);

			data.insert(new_it, data_ptr, data_ptr + sizeof(v));
		}
		
		void ready()
		{
			check(!packet, "Internal packet already exists");

			packet = enet_packet_create(data.data(), data.size(), flags);
		}

		ENetPacket* get_packet() const { return packet; }
	};

	using channel_dispatch_t = std::function<PacketResult(const PacketR&)>;

	void init();
	void add_channel_dispatcher(uint8_t id, const channel_dispatch_t& fn);
	void call_channel_dispatcher(const ENetEvent& id);

	inline bool send_packet(ENetPeer* peer, const void* data, size_t size, uint32_t flags = ENET_PACKET_FLAG_RELIABLE, uint8_t channel = ChannelID_Generic)
	{
		check(peer && data && size > 0, "Invalid peer or packet");

		const auto packet = enet_packet_create(data, size, flags);

		check(packet, "Net packet could not be created");

		return enet_peer_send(peer, channel, packet) == 0;
	}

	inline bool send_packet(ENetPeer* peer, ENetPacket* p, uint8_t channel = ChannelID_Generic)
	{
		check(peer && p, "Invalid peer or packet");

		return enet_peer_send(peer, channel, p) == 0;
	}

	template <typename Fn>
	inline void dispatch_packets(const Fn& fn, uint32_t timeout = 0)
	{
		ENetEvent e;

		while (enet_host_service(GET_HOST(), &e, timeout) > 0)
			fn(e);
	}

	template <typename Fn>
	inline void dispatch_packet(const Fn& fn, uint32_t timeout = 0)
	{
		ENetEvent e;

		if (enet_host_service(GET_HOST(), &e, timeout) > 0)
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