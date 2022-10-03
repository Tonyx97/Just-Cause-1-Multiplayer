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
	g_net->send_broadcast_joined(this, Packet(PlayerClientPID_ObjectInstanceSync, ChannelID_PlayerClient, player, NetObjectActionSyncType_Destroy));

	if (timed_out)
		logt(RED, "'{}' disconnected due to timeout (NID {:x})", player->get_nick(), get_nid());
	else logt(YELLOW, "'{}' disconnected (NID {:x})", player->get_nick(), get_nid());
#endif

	JC_FREE(player);
}

#ifdef JC_SERVER
#define SETUP_CREATE_SYNC_PACKET(player, joined) \
								const auto& dyn_info = player->get_dyn_info(); \
								Packet p(PlayerClientPID_ObjectInstanceSync, ChannelID_PlayerClient); \
								p.add(player, NetObjectActionSyncType_Create); \
								p.add(joined); \
								p.add(player->get_transform().pack()); \
								p.add(player->get_hp()); \
								p.add(player->get_max_hp()); \
								p.add(player->get_skin_info()); \
								p.add(dyn_info.skin); \
								p.add(dyn_info.nick)

void PlayerClient::startup_sync()
{
	// set player's default info before doing startup sync

	player->set_skin(0);
	player->set_hp(500.f);
	player->set_max_hp(500.f);

	// sync this player with other players

	sync_broadcast();

	// set the player as spawned
	
	player->spawn();
}

void PlayerClient::sync_broadcast()
{
	SETUP_CREATE_SYNC_PACKET(player, true);

	g_net->send_broadcast_joined(this, p);
}

void PlayerClient::sync_player(Player* target_player, bool create)
{
	if (player == target_player)
		return;

	if (create)
	{
		// sync creation/update

		SETUP_CREATE_SYNC_PACKET(target_player, false);

		send(p, true);
	}
	else
	{
		// sync hiding

		const auto& dyn_info = player->get_dyn_info();

		Packet p(PlayerClientPID_ObjectInstanceSync, ChannelID_PlayerClient);

		p.add(target_player, NetObjectActionSyncType_Hide);

		send(p, true);
	}
}

void PlayerClient::sync_entity(NetObject* target_entity, bool create)
{
	check(!target_entity->cast<Player>(), "{} does not support players", CURR_FN);

	Packet p(PlayerClientPID_ObjectInstanceSync, ChannelID_PlayerClient);

	if (create)
	{
		p.add(target_entity, NetObjectActionSyncType_Create);
		p.add(target_entity->get_object_id());
		p.add(target_entity->get_transform().pack());
		p.add(target_entity->get_hp());
		p.add(target_entity->get_max_hp());
	}
	else
	{
		p.add(target_entity, NetObjectActionSyncType_Hide);
	}

	send(p, true);
}

bool PlayerClient::compare_address(const ENetAddress& other)
{
	const auto address = get_address();

	return (in6_equal(address->host, other.host) && address->port == other.port);
}

std::string PlayerClient::get_ip() const
{
	char ip_str[64] = { 0 };

	enet_address_get_host_ip(&peer->address, ip_str, sizeof(ip_str));

	return ip_str;
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