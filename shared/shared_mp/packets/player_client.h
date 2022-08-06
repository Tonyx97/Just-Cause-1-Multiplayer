#pragma once

struct PlayerClientSyncInstancesPacket
{
	static constexpr PacketID ID = PlayerClientPID_SyncInstances;
	static constexpr PacketID CHANNEL = ChannelID_PlayerClient;
	static constexpr int RELIABILITY = ENET_PACKET_FLAG_RELIABLE;

ALIGN_PUSH(1)
	struct Info
	{
		NID nid = INVALID_NID;

		NetObjectType type = NetObject_Invalid;

		vec3 position;

		quat rotation;

		float hp,
			  max_hp;
	};
ALIGN_POP()

	std::vector<Info> net_objects;

#ifdef JC_SERVER
	std::vector<uint8_t> serialize() const
	{
		std::vector<uint8_t> data;

		enet::serialize_int(data, ID);
		enet::serialize_int(data, net_objects.size());

		for (const auto& obj : net_objects)
			enet::serialize_params(data, obj);

		return data;
	}

	PacketHolder serialize_as_packet() const { return PacketHolder(serialize(), RELIABILITY); }
#endif

#ifdef JC_CLIENT
	PlayerClientSyncInstancesPacket& deserialize(std::vector<uint8_t>& data)
	{
		const auto size = enet::deserialize_int<size_t>(data);

		for (size_t i = 0u; i < size; ++i)
		{
			DESERIALIZE_NID_AND_TYPE(data);

			const auto position = enet::deserialize_general_data<vec3>(data);
			const auto rotation = enet::deserialize_general_data<quat>(data);
			const float hp = enet::deserialize_float(data);
			const float max_hp = enet::deserialize_float(data);

			net_objects.emplace_back(nid, type, position, rotation, hp, max_hp);
		}

		return *this;
	}
#endif
};

struct PlayerClientStartupInfoPacket
{
	static constexpr PacketID ID = PlayerClientPID_StartupInfo;
	static constexpr PacketID CHANNEL = ChannelID_PlayerClient;
	static constexpr int RELIABILITY = ENET_PACKET_FLAG_RELIABLE;

ALIGN_PUSH(1)
	struct Info
	{
		std::string nick;

		Player::SkinInfo skin_info;

		int32_t skin;

		float hp,
			  max_hp;
	};
ALIGN_POP()

	std::vector<std::pair<Player*, Info>> info;

	std::vector<uint8_t> serialize() const
	{
		std::vector<uint8_t> data;

		enet::serialize_int(data, ID);
		enet::serialize_int(data, info.size());

		for (const auto& [player, _info] : info)
			enet::serialize_params(data,
				player,
				_info.nick,
				_info.skin_info.props,
				_info.skin_info.cloth_skin,
				_info.skin_info.head_skin,
				_info.skin_info.cloth_color,
				_info.skin,
				_info.hp,
				_info.max_hp);

		return data;
	}

	PacketHolder serialize_as_packet() const { return PacketHolder(serialize(), RELIABILITY); }

	PlayerClientStartupInfoPacket& deserialize(std::vector<uint8_t>& data)
	{
		const auto size = enet::deserialize_int<size_t>(data);

		for (size_t i = 0u; i < size; ++i)
		{
			const auto player = enet::deserialize_net_object(data)->cast_safe<Player>();

			Info _info
			{
				.nick = enet::deserialize_string(data),
				.skin_info =
				{
					.props = enet::deserialize_vector<VariantPropInfo>(data),
					.cloth_skin = enet::deserialize_int(data),
					.head_skin = enet::deserialize_int(data),
					.cloth_color = enet::deserialize_int(data),
				},
				.skin = enet::deserialize_int(data),
				.hp = enet::deserialize_float(data),
				.max_hp = enet::deserialize_float(data),
			};

			info.emplace_back(player, _info);
		}

		return *this;
	}
};