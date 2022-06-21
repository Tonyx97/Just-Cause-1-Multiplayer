#include <defs/standard.h>

#include "nh_init.h"

#include <player_client/player_client.h>

enet::PacketResult nh::init::init(const enet::PacketR& p)
{
	const auto pc = p.get_player_client_owner();
	const auto nick = p.get_str<std::string>();

	pc->set_nick(nick);

	logt(YELLOW, "Player '{}' requesting init info", nick);

	// send the init information to the client

	enet::send_reliable<ChannelID_Init>(p.get_peer(), p.get_id(), pc->get_nid());

	return enet::PacketRes_Ok;
}