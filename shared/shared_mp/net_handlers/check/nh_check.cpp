#include <defs/standard.h>

#include "nh_check.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::check::net_objects(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto count = p.get_int();

	log(YELLOW, "[{}] {}", CURR_FN, count);

	for (int i = 0; i < count; ++i)
	{
		DESERIALIZE_NID_AND_TYPE(p);

		if (g_net->get_net_object_by_nid(nid))
		{
			log(YELLOW, "Net object with type {} and NID {:x} already exists", type, nid);

			continue;
		}

		switch (type)
		{
		case NetObject_Player:
		{
			const auto new_player = g_net->add_player_client(nid);

			log(YELLOW, "Created new player from net objects check: {:x}", new_player->get_nid());

			break;
		}
		default:
			log(RED, "Unknown type of net object at '{}' ({})", CURR_FN, type);
		}
	}
#else
	enet::PacketW out_p(CheckPID_NetObjects);

	out_p.add(g_net->get_net_objects_count());

	g_net->for_each_net_object([&](NID nid, NetObject* obj)
	{
		out_p.add(obj);
	});

	out_p.ready();

	p.get_pc()->send<ChannelID_Check>(out_p);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::check::players_static_info(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto count = p.get_int();

	log(YELLOW, "[{}] {}", CURR_FN, count);

	for (int i = 0; i < count; ++i)
	{
		if (const auto player = p.get_net_object<Player>())
		{
			const auto nick = p.get_str();

			player->set_nick(nick);

			log(PURPLE, "Updated nick for player {:x} -> '{}'", player->get_nid(), nick);
		}
	}
#else
	g_net->send_players_info();
#endif

	return enet::PacketRes_Ok;
}