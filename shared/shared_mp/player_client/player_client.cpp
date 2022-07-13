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
	// the player client is not added yet in the list so it will broadcast
	// to the rest of players

	g_net->send_broadcast_reliable<ChannelID_PlayerClient>(this, PlayerClientPID_Quit, player);

	if (timed_out)
		logt(RED, "'{}' disconnected due to timeout (NID {:x})", player->get_nick(), get_nid());
	else logt(YELLOW, "'{}' disconnected (NID {:x})", player->get_nick(), get_nid());
#endif

	JC_FREE(player);
}

void PlayerClient::startup_sync()
{
}

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
	{
		// sync net object instances

		{
			PlayerClientSyncInstancesPacket p;

			g_net->for_each_net_object([&](NID, NetObject* obj)
			{
				if (auto casted_player = obj->cast<Player>())
				{
					if (casted_player->get_client()->is_joined())
					{
						p.net_objects.push_back(obj);

						log(PURPLE, "Syncing player with NID {:x} ({})", casted_player->get_nid(), casted_player->get_nick());
					}
				}
				else
				{
					p.net_objects.push_back(obj);

					log(PURPLE, "Syncing net object with NID {:x}", obj->get_nid());
				}
			});

			// send all net object main info to all players who joined to sync instances
			// this creates and spawns the players that are not in other players'
			// game, for example, if a player just joined, it will create the player for 
			// the other players who already joined and it will also create those players for
			// this player

			g_net->send_broadcast_joined_reliable(p);
		}

		// sync player's static info

		{
			PlayerClientStaticInfoPacket p;

			g_net->for_each_joined_player_client([&](NID, PlayerClient* pc)
			{
				if (auto player = pc->get_player())
				{
					const auto& static_info = player->get_static_info();

					p.info.emplace_back(player, PlayerClientStaticInfoPacket::Info
					{
						.nick = static_info.nick,
						.skin = static_info.skin
					});

					log(PURPLE, "Updating player static info with NID {:x} ({})", player->get_nid(), player->get_nick());
				}
			});

			// send the static info of each player to all players

			g_net->send_broadcast_joined_reliable(p);
		}

		// let the other players know this player joined

		g_net->send_broadcast_joined_reliable<ChannelID_PlayerClient>(this, PlayerClientPID_Join, player);
	}
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