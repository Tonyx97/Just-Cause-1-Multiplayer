#include <defs/standard.h>

#include "nh_init.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::init::init(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto nid = p.get_int<NID>();

	// create local player and set the client ready

	g_net->add_local(nid);
	g_net->set_ready();

	log(GREEN, "Init packet received (NID: {:x})", nid);
	log(GREEN, "Localplayer PC: {} ({})", (void*)g_net->get_local(), g_net->get_local()->get_nick());
	log(GREEN, "Localplayer: {}", (void*)g_net->get_local()->get_player());

	// ask the server to check for net objects and create the proper instances locally,
	// also make sure we retrieve the static info for players too
	
	enet::send_reliable<ChannelID_Check>(CheckPID_NetObjects);
	enet::send_reliable<ChannelID_Check>(CheckPID_PlayersStaticInfo);
#else
	const auto pc = p.get_player_client_owner();
	const auto nick = p.get_str<std::string>();

	logt(YELLOW, "Player '{}' requesting init info", nick);

	// send the init information to the client

	enet::send_reliable<ChannelID_Init>(p.get_peer(), p.get_id(), pc->get_nid());

	// update player client and broadcast the player to the rest

	pc->set_nick(nick);
	pc->set_ready();
#endif

	return enet::PacketRes_Ok;
}