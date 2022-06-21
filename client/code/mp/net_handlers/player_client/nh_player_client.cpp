#include <defs/standard.h>

#include "nh_player_client.h"

enet::PacketResult nh::player_client::connect(const enet::PacketR& p)
{
	const auto player_name = p.get_str(); // todojc

	log(YELLOW, "[{}] {}", CURR_FN, player_name);

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::disconnect(const enet::PacketR& p)
{
	const auto player_name = p.get_str(); // todojc

	log(YELLOW, "[{}] {}", CURR_FN, player_name);

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::nick(const enet::PacketR& p)
{
	const auto player_name = p.get_str();  // todojc
	const auto nick = p.get_str();  // todojc

	log(YELLOW, "[{}] {}", CURR_FN, nick);

	return enet::PacketRes_Ok;
}