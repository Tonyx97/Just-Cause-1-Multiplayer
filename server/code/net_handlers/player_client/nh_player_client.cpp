#include <defs/standard.h>

#include "nh_player_client.h"

enet::PacketResult nh::player_client::nick(const enet::PacketR& p)
{
	const auto nick = p.get_str();
	const auto pc	= p.get_player_client();

	pc->set_nick(nick);

	return enet::PacketRes_Ok;
}