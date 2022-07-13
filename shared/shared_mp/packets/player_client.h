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

struct PlayerClientStaticInfoPacket
{
	struct Info
	{
		std::string nick;

		uint32_t skin;
	};

	static constexpr uint32_t ID = PlayerClientPID_StaticInfo;
	static constexpr uint32_t CHANNEL = ChannelID_PlayerClient;

	std::vector<std::pair<Player*, Info>> info;

	vec<uint8_t> serialize() const
	{
		vec<uint8_t> data;

		enet::serialize_int(data, ID);
		enet::serialize_int(data, info.size());

		for (const auto& [player, _info] : info)
			enet::serialize_params(data,
				player,
				_info.nick,
				_info.skin);

		return data;
	}

	PacketHolder serialize_as_packet() const { return { serialize() }; }

	PlayerClientStaticInfoPacket& deserialize(vec<uint8_t>& data)
	{
		const auto size = enet::deserialize_int(data);

		for (int i = 0; i < size; ++i)
		{
			const auto player = enet::deserialize_net_object(data)->cast_safe<Player>();

			Info _info
			{
				.nick = enet::deserialize_string(data),
				.skin = enet::deserialize_int<uint32_t>(data)
			};

			info.emplace_back(player, _info);
		}

		return *this;
	}
};