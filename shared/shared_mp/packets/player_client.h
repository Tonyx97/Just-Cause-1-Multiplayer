#pragma once

struct PlayerClientSyncInstancesPacket
{
	static constexpr uint32_t ID = PlayerClientPID_SyncInstances;
	static constexpr uint32_t CHANNEL = ChannelID_PlayerClient;

#ifdef JC_CLIENT
	std::vector<std::pair<NID, uint32_t>> net_objects;
#else
	std::vector<NetObject*> net_objects;
#endif

#ifdef JC_SERVER
	vec<uint8_t> serialize() const
	{
		vec<uint8_t> data;

		enet::serialize_int(data, ID);
		enet::serialize_int(data, net_objects.size());

		for (auto obj : net_objects)
			enet::serialize_net_object(data, obj);

		return data;
	}

	PacketHolder serialize_as_packet() const { return { serialize() }; }
#endif

#ifdef JC_CLIENT
	PlayerClientSyncInstancesPacket& deserialize(vec<uint8_t>& data)
	{
		const auto size = enet::deserialize_int(data);

		for (int i = 0; i < size; ++i)
		{
			DESERIALIZE_NID_AND_TYPE(data);

			net_objects.emplace_back(nid, type);
		}

		return *this;
	}
#endif
};