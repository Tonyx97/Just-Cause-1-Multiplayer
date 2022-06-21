#include <defs/standard.h>

#include <enet/interface.h>

#include "player_client.h"

PlayerClient::PlayerClient(ENetPeer* peer) : peer(peer)
{
	enet_peer_timeout(peer, 0, 0, enet::PEER_TIMEOUT);
	
	logt(GREEN, "Player client connected (NID {:x})", get_nid());

	enet::send_broadcast_reliable<ChannelID_PlayerClient>(PlayerClientPID_Connect, this);
}

PlayerClient::~PlayerClient()
{
	enet::send_broadcast_reliable<ChannelID_PlayerClient>(PlayerClientPID_Disconnect, this);

	if (timed_out)
		logt(RED, "'{}' disconnected due to timeout (NID {:x})", nick, get_nid());
	else logt(YELLOW, "'{}' disconnected (NID {:x})", nick, get_nid());
}

void PlayerClient::set_nick(const std::string& v)
{
	nick = v;
}