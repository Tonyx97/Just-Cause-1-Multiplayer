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
	Packet p(PlayerClientPID_Quit, ChannelID_PlayerClient, player);

	g_net->send_broadcast(p);

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

	// sync net object instances

	/*{
		PlayerClientSyncInstancesPacket p;

		g_net->for_each_net_object([&](NID, NetObject* obj)
		{
			NetObject* valid_net_object = nullptr;

			if (auto casted_player = obj->cast<Player>())
			{
				if (casted_player->get_client()->is_joined())
				{
					valid_net_object = casted_player;

					log(PURPLE, "Syncing player with NID {:x} ({})", casted_player->get_nid(), casted_player->get_nick());
				}
			}
			else
			{
				valid_net_object = obj;

				log(PURPLE, "Syncing net object with NID {:x}", obj->get_nid());
			}

			if (valid_net_object)
			{
				p.net_objects.push_back(
				{
					.nid = valid_net_object->get_nid(),
					.type = valid_net_object->get_type(),
					.position = valid_net_object->get_position(),
					.rotation = valid_net_object->get_rotation(),
					.hp = valid_net_object->get_hp(),
					.max_hp = valid_net_object->get_max_hp(),
					.object_id = valid_net_object->get_object_id(),
				});
			}
		});

		// send all net object main info to all players who joined to sync instances
		// this creates and spawns the players that are not in other players'
		// game, for example, if a player just joined, it will create the player for 
		// the other players who already joined and it will also create those players for
		// this player

		g_net->send_broadcast_joined(p);
	}

	// sync player's basic info

	{
		PlayerClientStartupInfoPacket p;

		g_net->for_each_joined_player_client([&](NID, PlayerClient* pc)
		{
			if (auto player = pc->get_player())
			{
				const auto& dyn_info = player->get_dyn_info();

				p.info.emplace_back(player, PlayerClientStartupInfoPacket::Info
				{
					.nick = dyn_info.nick,
					.skin_info = player->get_skin_info(),
					.skin = dyn_info.skin,
					.hp = player->get_hp(),
					.max_hp = player->get_max_hp(),
				});

				log(PURPLE, "Updating player basic info with NID {:x} ({})", player->get_nid(), player->get_nick());
			}
		});

		// send the basic info of each player to all players

		g_net->send_broadcast_joined(p);
	}

	// let the other players know this player joined
		
	g_net->send_broadcast_joined_reliable<ChannelID_PlayerClient>(this, PlayerClientPID_Join, player);*/

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