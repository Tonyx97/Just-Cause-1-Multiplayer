#include <defs/standard.h>

#include "nh_player_client.h"

#include <player_client/player_client.h>

enet::PacketResult nh::player_client::nick(const enet::PacketR& p)
{
	const auto nick = p.get_str<std::string>();
	const auto pc	= p.get_player_client_owner();

	pc->set_nick(nick);

	return enet::PacketRes_Ok;
}