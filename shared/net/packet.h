#pragma once

enum PacketResult
{
	PacketRes_Ok,
	PacketRes_NotFound,
	PacketRes_NotUsable,
	PacketRes_BadArgs,
	PacketRes_NotSupported,
	PacketRes_NotAllowed,
};

class Packet
{
private:

	mutable serialization_ctx ctx {};

#ifdef JC_SERVER
	PlayerClient*	pc = nullptr;
#endif

	mutable ENetPacket* packet = nullptr;
	ENetPeer*		peer = nullptr;
	float			time = 0.f;
	uint32_t		flags = 0;
	PacketID		id = 0u;
	uint8_t			channel = 0u;

public:

	Packet(const ENetEvent& e)
		: peer(e.peer)
		, packet(e.packet)
		, channel(e.channelID)
		, time(util::time::get_time())
	{
		// add the packet data to the serialization context

		ctx.set_data(e.packet->data, e.packet->dataLength);

		// always get the packet id first

		id = get<PacketID>();
		flags = packet->flags;

#ifdef JC_SERVER
		pc = BITCAST(PlayerClient*, e.peer->data);
#endif
	}

	template <typename... A>
	Packet(const Packet& other, A&&... args)
		: id(other.id)
		, channel(other.channel)
		, time(util::time::get_time())
	{
		flags = other.flags;
		peer = other.peer;

#ifdef JC_SERVER
		pc = other.pc;
#endif

		// always add the packet id when creating a new packet

		add(id, args...);
	}

	template <typename... A>
	Packet(PacketID	pid, ChannelID channel, A&&... args)
		: id(pid)
		, channel(channel)
		, time(util::time::get_time())
	{
		set_reliable();

		// always add the packet id when creating a new packet

		add(id, args...);
	}

	~Packet()
	{
		check(packet, "Packet was already destroyed or it's invalid");

		if (packet->referenceCount > 0)
			return;

		enet_packet_destroy(packet);

		packet = nullptr;
	}

	Packet(const Packet& other) = delete;
	Packet(Packet&& other) = delete;
	Packet& operator=(Packet&& other) = delete;

	void create() const
	{
		if (!packet)
			packet = enet_packet_create(ctx.data.data(), ctx.data.size(), flags);
		else
		{
			check(packet->referenceCount == 0, "Packet could not be rebuild");

			enet_packet_destroy(packet);

			packet = nullptr;

			create();
		}
	}

	Packet& set_reliable(bool v) { return v ? set_reliable() : set_unreliable(); }
	Packet& set_reliable() { flags = ENET_PACKET_FLAG_RELIABLE; return *this; }
	Packet& set_unreliable() { flags = 0; return *this; }
	Packet& set_unsequenced() { flags = ENET_PACKET_FLAG_UNSEQUENCED; return *this; }

	ENetPeer* get_peer() const { return peer; }

#ifdef JC_SERVER
	PlayerClient* get_pc() const { return pc; }
#endif

	uint8_t get_id() const { return id; }
	uint8_t get_channel() const { return channel; }

	int32_t get_flags() const { return packet->flags; }

	float get_time() const { return time; }

	ENetPacket* get_packet() const { return packet; }

	template <typename... A>
	void add(A&&... args) const
	{
		serialize(ctx, args...);
	}

	template <typename... A>
	void add_beginning(A&&... args) const
	{
		ctx.write_offset = sizeof(PacketID);
		add(args...);
		ctx.write_offset = -1;
	}

	template <typename T>
	T get() const { return deserialize<T>(ctx); }

	bool get_bool() const { return get<bool>(); }

	uint8_t get_u8() const { return get<uint8_t>(); }
	int8_t get_i8() const { return get<uint8_t>(); }

	uint16_t get_u16() const { return get<uint16_t>(); }
	int16_t get_i16() const { return get<int16_t>(); }

	uint32_t get_u32() const { return get<uint32_t>(); }
	int32_t get_i32() const { return get<int32_t>(); }

	float get_float() const { return get<float>(); }

	uint64_t get_u64() const { return get<uint64_t>(); }
	int64_t get_i64() const { return get<int64_t>(); }

	template <typename T = NetObject>
	T* get_net_object() const
	{
		if (const auto net_obj = get<NetObject*>())
		{
			if constexpr (std::is_same_v<T, NetObject>)
				return net_obj;
			else return net_obj->cast_safe<T>();
		}

		return nullptr;
	}

	std::pair<NID, NetObjectType> get_nid_and_type() const
	{
		const auto _nid = get<NID>();
		const auto _type = get<NetObjectType>();

		return { _nid, _type };
	}

	std::string get_str() const { return get<std::string>(); }

	std::wstring get_wstr() const { return get<std::wstring>(); }
};