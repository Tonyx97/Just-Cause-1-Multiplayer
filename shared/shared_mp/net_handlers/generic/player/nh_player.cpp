#include <defs/standard.h>

#include "nh_player.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

// debug

#ifdef JC_CLIENT
#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/base/comps/physical.h>
#endif

enet::PacketResult nh::player::spawn(const enet::Packet& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
	{
		const auto hp = p.get_float(),
				   max_hp = p.get_float();

		player->spawn();
		player->set_hp(hp);
		player->set_max_hp(max_hp);
	}
#else
	// maybe we need this at some point (receive spawn packet from client)
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::dynamic_info(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	// dispatch all dynamic infos sent in this packet

	while (!p.is_empty())
	{
		switch (const auto type = p.get_uint())
		{
		case PlayerDynInfo_Transform:
		{
			auto transform = p.get_raw<Transform>();

			player->set_transform(transform);

#ifdef JC_SERVER
			g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, transform);
#endif

			break;
		}
		case PlayerDynInfo_Velocity:
		{
			const auto velocity = p.get_raw<vec3>();

			//player->get_character()->get_physical()->set_velocity(velocity);

#ifdef JC_SERVER
			g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, velocity);
#endif

			break;
		}
		case PlayerDynInfo_HeadRotation:
		{
			const auto rotation = p.get_raw<vec3>();

			player->set_head_rotation(rotation);

#ifdef JC_SERVER
			g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, rotation);
#endif

			break;
		}
		case PlayerDynInfo_Skin:
		{
			const auto skin_id = p.get_uint();

			player->set_skin(skin_id);

#ifdef JC_SERVER
			g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, skin_id);
#endif
		}
		}
	}

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::stance_and_movement(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();
	
	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	switch (const auto type = p.get_uint())
	{
	case PlayerStanceID_Movement:
	{
		const float angle = p.get_float(),
					right = p.get_float(),
					forward = p.get_float();

		const bool aiming = p.get_bool();

		player->set_movement_info(angle, right, forward, aiming);

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, angle, right, forward, aiming);
#endif

		break;
	}
	case PlayerStanceID_Jump:
	{
		player->set_body_stance_id(BodyStance_Jump);

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type);
#endif

		break;
	}
	case PlayerStanceID_Punch:
	{
		player->do_punch();

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type);
#endif

		break;
	}
	case PlayerStanceID_BodyStance:
	{
		const auto stance_id = p.get_uint();

#ifdef JC_CLIENT
		player->set_body_stance_id(stance_id);
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, stance_id);
#endif

		break;
	}
	}

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::health(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;

#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const float new_hp = p.get_float();

	player->set_hp(new_hp);

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_Health, player, new_hp);
#endif

	return enet::PacketRes_Ok;
}