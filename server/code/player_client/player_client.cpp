#include <defs/standard.h>

#include "player_client.h"

PlayerClient::PlayerClient(ENetPeer* peer) : peer(peer)
{
	enet_peer_timeout(peer, 0, 0, enet::PEER_TIMEOUT);

	nid = enet::GET_FREE_NID();
	
	logt(GREEN, "Player client connected (NID {:x})", nid);
}

PlayerClient::~PlayerClient()
{
	if (timed_out)
		logt(RED, "'{}' disconnected due to timeout (NID {:x})", nick, nid);
	else logt(YELLOW, "'{}' disconnected (NID {:x})", nick, nid);

	enet::FREE_NID(nid);
}

void PlayerClient::set_nick(const std::string& v)
{
	nick = v;
}