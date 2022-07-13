#include <defs/standard.h>

#include "nh_player.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

// debug

#ifdef JC_CLIENT
#include <game/object/character/character.h>
#endif

enet::PacketResult nh::player::spawn(const enet::Packet& p)
{
#ifdef JC_CLIENT
	// todojc

	/*if (const auto player = p.get_net_object<Player>())
		player->spawn();*/
#else
	// maybe we need this at some point (receive spawn packet from client)
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::tick_info(const enet::Packet& p)
{
#ifdef JC_CLIENT
	/*if (const auto player = p.get_net_object<Player>())
	{
		const auto info = p.get_struct<Player::TickInfo>();

		player->set_tick_info(info);
	}*/
#else
	/*const auto pc = p.get_pc();
	const auto player = pc->get_player();
	const auto info = p.get_struct<Player::TickInfo>();

	player->set_tick_info(info);

	g_net->send_broadcast_reliable(pc, PlayerPID_TickInfo, player, info);*/
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::dynamic_info(const enet::Packet& p)
{
/*#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	switch (const auto type = p.get_uint())
	{
	case 0: // head rotation
	{
		const auto head_rotation = p.get_struct<vec3>();

#ifdef JC_CLIENT
		player->get_character()->get_skeleton()->set_head_euler_rotation(head_rotation);
#endif

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, head_rotation.x, head_rotation.y, head_rotation.z);
#endif

		break;
	}
	}*/

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::stance(const enet::Packet& p)
{
/*#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();
	
	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const bool is_body = p.get_bool();
	const auto stance_id = p.get_uint();

	if (is_body)
		player->set_body_stance_id(stance_id);
	else player->set_arms_stance_id(stance_id);

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_Stance, player, is_body, stance_id);
#endif*/

	return enet::PacketRes_Ok;
}