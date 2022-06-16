#include <defs/standard.h>

#include "player_client.h"

PlayerClient::PlayerClient(ENetPeer* peer) : peer(peer)
{
	enet_peer_timeout(peer, 0, 0, enet::PEER_TIMEOUT);
	
	logt(GREEN, "Player client connected");
}

PlayerClient::~PlayerClient()
{
	if (timed_out)
		logt(RED, "'{}' disconnected due to timeout", nick);
	else logt(YELLOW, "'{}' disconnected", nick);
}

void PlayerClient::set_nick(const std::string& v)
{
	nick = v;
}