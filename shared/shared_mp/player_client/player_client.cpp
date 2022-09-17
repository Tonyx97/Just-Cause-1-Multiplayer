#include <defs/standard.h>

#include <mp/net.h>

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
}
#endif

PlayerClient::~PlayerClient()
{
#ifdef JC_SERVER
	g_net->send_broadcast_joined(this, Packet(PlayerClientPID_Quit, ChannelID_PlayerClient, player));

	if (timed_out)
		logt(RED, "'{}' disconnected due to timeout (NID {:x})", player->get_nick(), get_nid());
	else logt(YELLOW, "'{}' disconnected (NID {:x})", player->get_nick(), get_nid());
#endif

	JC_FREE(player);
}

#ifdef JC_SERVER
void PlayerClient::startup_sync()
{
	// set player's default info before doing startup sync

	player->set_skin(0);
	player->set_hp(500.f);
	player->set_max_hp(500.f);

	// let the other players know this player joined
		
	g_net->send_broadcast_joined(this, Packet(PlayerClientPID_Join, ChannelID_PlayerClient, player));

	// set the player as spawned
	
	player->spawn();
}

bool PlayerClient::compare_address(const ENetAddress& other)
{
	const auto address = get_address();

	return (in6_equal(address->host, other.host) && address->port == other.port);
}
#endif

void PlayerClient::set_initialized(bool v)
{
	initialized = v;
}

void PlayerClient::set_joined(bool v)
{
	joined = v;

#ifdef JC_CLIENT
#else
	if (joined)
		startup_sync();
#endif
}

void PlayerClient::set_nick(const std::string& v)
{
	player->set_nick(v);
}

NID PlayerClient::get_nid() const
{
	return player->get_nid();
}

const std::string& PlayerClient::get_nick() const
{
	return player->get_nick();
}