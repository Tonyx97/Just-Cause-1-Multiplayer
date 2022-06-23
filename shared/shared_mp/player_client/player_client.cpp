#ifndef JC_LOADER
#include <defs/standard.h>

#include <net/interface.h>

#include "player_client.h"

#include <shared_mp/objs/player.h>

#ifdef JC_CLIENT
PlayerClient::PlayerClient(NID nid)
{
	player = JC_ALLOC(Player, this, nid);
}
#else
PlayerClient::PlayerClient(ENetPeer* peer) : peer(peer)
{
	enet_peer_timeout(peer, 0, 0, enet::PEER_TIMEOUT);

	player = JC_ALLOC(Player, this);

	logt(GREEN, "Player client connected (NID {:x})", get_nid());

	enet::send_broadcast_reliable<ChannelID_PlayerClient>(PlayerClientPID_Connect, player);
}
#endif

PlayerClient::~PlayerClient()
{
#ifdef JC_SERVER
	enet::send_broadcast_reliable<ChannelID_PlayerClient>(PlayerClientPID_Disconnect, player);

	if (timed_out)
		logt(RED, "'{}' disconnected due to timeout (NID {:x})", nick, get_nid());
	else logt(YELLOW, "'{}' disconnected (NID {:x})", nick, get_nid());
#endif

	JC_FREE(player);
}

void PlayerClient::set_nick(const std::string& v)
{
	nick = v;
}

NID PlayerClient::get_nid() const
{
	return player->get_nid();
}
#endif