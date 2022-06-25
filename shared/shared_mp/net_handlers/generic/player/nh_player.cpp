#include <defs/standard.h>

#include "nh_player.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::player::sync_spawn(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
		player->sync_spawn();
#else
	// maybe we need this at some point (receive spawn packet from client)
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::transform(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
		player->set_transform(p.get_struct<Transform>());
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();

	//player->set_transform();

	g_net->send_broadcast_reliable(pc, PlayerPID_Transform, player, p.get_struct<Transform>());
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::set_anim(const enet::PacketR& p)
{
	return enet::PacketRes_Ok;
}