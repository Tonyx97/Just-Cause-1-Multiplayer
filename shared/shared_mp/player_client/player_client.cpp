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

	// the player client is not added yet in the list so it will broadcast
	// to the rest of players

	g_net->send_broadcast_reliable<ChannelID_PlayerClient>(this, PlayerClientPID_Connect, player);
}
#endif

PlayerClient::~PlayerClient()
{
#ifdef JC_SERVER
	// the player client is not added yet in the list so it will broadcast
	// to the rest of players

	g_net->send_broadcast_reliable<ChannelID_PlayerClient>(this, PlayerClientPID_Disconnect, player);

	if (timed_out)
		logt(RED, "'{}' disconnected due to timeout (NID {:x})", player->get_nick(), get_nid());
	else logt(YELLOW, "'{}' disconnected (NID {:x})", player->get_nick(), get_nid());
#endif

	JC_FREE(player);
}

void PlayerClient::set_nick(const std::string& v)
{
	player->set_nick(v);
}

#ifdef JC_SERVER
void PlayerClient::set_state(uint32_t v, const enet::PacketR* p)
{
	switch (state = v)
	{
	case PCState_Initializing:
	{
		set_nick(p->get_str());
		send_reliable<ChannelID_PlayerClient>(PlayerClientPID_State, state, get_nid());

		logt(YELLOW, "Player {:x} initializing (nick: {})", get_nid(), get_nick());

		break;
	}
	case PCState_Initialized:
	{
		logt(YELLOW, "Player {:x} initialized", get_nid());

		break;
	}
	case PCState_Loaded:
	{
		// sync net objects instances when this player loads
		// and also sync all players static info and spawning

		g_net->sync_net_objects_instances_for_player(this);
		g_net->sync_players_static_info();
		g_net->sync_players_spawn();

		break;
	}
	}
}
#endif

NID PlayerClient::get_nid() const
{
	return player->get_nid();
}

const std::string& PlayerClient::get_nick() const
{
	return player->get_nick();
}