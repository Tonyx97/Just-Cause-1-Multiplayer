#include <defs/standard.h>

#include "nh_player_client.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::player_client::connect(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	DESERIALIZE_NID_AND_TYPE(p);

	check(g_net->get_local()->get_nid() != nid, "A localplayer cannot receive 'connect' packet with its NID");
	check(!g_net->get_player_by_nid(nid), "Player must not exist before '{}' packet", CURR_FN);

	const auto new_player = g_net->add_player_client(nid);

	log(YELLOW, "[{}] Created new player from connection: {:x}", CURR_FN, new_player->get_nid());
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::disconnect(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	check(g_net->get_local()->get_nid() != player->get_nid(), "A localplayer cannot receive 'connect' packet with its NID");
	check(player, "Player must exist before '{}' packet", CURR_FN);

	log(YELLOW, "[{}] Player {:x} removed", CURR_FN, player->get_nid());

	g_net->remove_player_client(player->get_client());
#endif

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