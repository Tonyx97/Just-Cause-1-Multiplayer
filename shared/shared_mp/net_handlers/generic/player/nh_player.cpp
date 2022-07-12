#include <defs/standard.h>

#include "nh_player.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::player::spawn(const enet::PacketR& p)
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

enet::PacketResult nh::player::tick_info(const enet::PacketR& p)
{
	auto info = p.get_struct<Player::TickInfo>();

#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
		player->set_tick_info(info);
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();

	player->set_tick_info(info);

	g_net->send_broadcast_reliable(pc, PlayerPID_TickInfo, player, info);
#endif

	return enet::PacketRes_Ok;
}