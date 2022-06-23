#include <defs/standard.h>

#include "nh_player_client.h"

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::player_client::connect(const enet::PacketR& p)
{
	if (const auto player = p.get_net_object<Player>())
	{
		log(YELLOW, "[{}] {}", CURR_FN, player->get_nid());
	}

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::disconnect(const enet::PacketR& p)
{
	if (const auto player = p.get_net_object<Player>())
	{
		log(YELLOW, "[{}] {}", CURR_FN, player->get_nid());
	}

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::nick(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
	{
		const auto nick = p.get_str<std::string>();

		player->get_client()->set_nick(nick);

		log(YELLOW, "[{}] {} {}", CURR_FN, player->get_nid(), nick);
	}
#else
	const auto nick = p.get_str<std::string>();
	const auto pc = p.get_player_client_owner();

	pc->set_nick(nick);
#endif

	return enet::PacketRes_Ok;
}