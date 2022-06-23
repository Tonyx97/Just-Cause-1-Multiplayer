#include <defs/standard.h>

#include "nh_player_client.h"

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::player_client::connect(const enet::PacketR& p)
{
	/*const auto player_name = p.get_str(); // todojc

	log(YELLOW, "[{}] {}", CURR_FN, player_name);*/

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::disconnect(const enet::PacketR& p)
{
	/*const auto player_name = p.get_str(); // todojc

	log(YELLOW, "[{}] {}", CURR_FN, player_name);*/

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::nick(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	/*const auto player_name = p.get_str();  // todojc
	const auto nick = p.get_str();  // todojc

	log(YELLOW, "[{}] {}", CURR_FN, nick);*/
#else
	const auto nick = p.get_str<std::string>();
	const auto pc = p.get_player_client_owner();

	pc->set_nick(nick);
#endif

	return enet::PacketRes_Ok;
}