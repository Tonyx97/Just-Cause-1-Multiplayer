#include <defs/standard.h>

#include "nh_check.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::check::net_objects(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto count = p.get_int();
	const auto localplayer = g_net->get_localplayer();

	log(YELLOW, "[{}] {}", CURR_FN, count);

	for (int i = 0; i < count; ++i)
	{
		DESERIALIZE_NID_AND_TYPE(p);

		if (localplayer->equal(nid))
			continue;

		if (g_net->get_net_object_by_nid(nid))
		{
			log(YELLOW, "Net object with NID {:x} already exists (type {})", nid, type);

			continue;
		}

		switch (type)
		{
		case NetObject_Player:
		{
			const auto new_player = g_net->add_player_client(nid);

			log(YELLOW, "[{}] Created new player with NID {:x}", CURR_FN, new_player->get_nid());

			break;
		}
		default:
			log(RED, "Unknown type of net object at '{}' ({})", CURR_FN, type);
		}
	}
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::check::players_static_info(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto count = p.get_int();
	const auto localplayer = g_net->get_localplayer();

	log(YELLOW, "[{}] {}", CURR_FN, count);

	for (int i = 0; i < count; ++i)
	{
		if (const auto player = p.get_net_object<Player>())
		{
			const auto info = p.get_struct<PacketCheck_PlayerStaticInfo>();

			if (localplayer->equal(player))
				continue;

			player->set_nick(*info.nick);
			player->set_skin(info.skin);

			log(PURPLE, "Updated static info for player {:x} ({} - {})", player->get_nid(), player->get_nick(), player->get_skin());
		}
	}
#endif

	return enet::PacketRes_Ok;
}