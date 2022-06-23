#ifndef JC_LOADER
#include <defs/standard.h>

#include <net/interface.h>

#include "player_client.h"

#ifdef JC_CLIENT
PlayerClient::PlayerClient(NID nid)
{
	set_nid(nid);
}
#else
PlayerClient::PlayerClient(ENetPeer* peer) : peer(peer)
{
	enet_peer_timeout(peer, 0, 0, enet::PEER_TIMEOUT);

	logt(GREEN, "Player client connected (NID {:x})", get_nid());

	enet::send_broadcast_reliable<ChannelID_PlayerClient>(PlayerClientPID_Connect, this);
}
#endif

PlayerClient::~PlayerClient()
{
#ifdef JC_SERVER
	enet::send_broadcast_reliable<ChannelID_PlayerClient>(PlayerClientPID_Disconnect, this);

	if (timed_out)
		logt(RED, "'{}' disconnected due to timeout (NID {:x})", nick, get_nid());
	else logt(YELLOW, "'{}' disconnected (NID {:x})", nick, get_nid());
#endif
}

void PlayerClient::set_nick(const std::string& v)
{
	nick = v;
}
#endif