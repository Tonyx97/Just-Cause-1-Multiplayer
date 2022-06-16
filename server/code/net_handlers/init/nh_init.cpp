#include <defs/standard.h>

#include "nh_init.h"

enet::PacketResult nh::init::dispatch(const enet::PacketR& p)
{
	const auto pc = p.get_player_client();
	const auto nick = p.get_str();

	pc->set_nick(nick);

	logt(YELLOW, "Player '{}' requesting init info", nick);

	// send the init information to the client

	enet::PacketW sp(p.get_id());

	sp.add("verified");
	sp.send(p.get_peer(), ChannelID_Init);

	return enet::PacketRes_Ok;
}