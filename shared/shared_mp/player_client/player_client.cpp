#include <defs/standard.h>

#include <mp/net.h>

#include "player_client.h"

#include <shared_mp/objs/player.h>

#ifdef JC_SERVER
#include <tcp_server.h>
#include <resource_sys/resource_system.h>
#endif

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

void PlayerClient::add_resource_to_sync(Resource* rsrc)
{
	resources_to_sync.push(rsrc->get_name());
}

void PlayerClient::sync_pending_resources()
{
	std::lock_guard lock(tcp_mtx);

	// if TCP connection is invalid then ignore
	// since we need need TCP to sync resources

	if (!tcp)
		return;
	
	// get and pop all the pending resources and send a packet to the client
	// containing info about the resource the server wants to sync

	if (const auto resources_count = resources_to_sync.size(); resources_count > 0u)
	{
		// send sync packet per resource

		g_rsrc->exec_with_resource_lock([&]()
		{
			// send resource metadata list

			{
				serialization_ctx out;

				_serialize(out, resources_to_sync.size());

				resources_to_sync.for_each([&](const std::string& rsrc_name)
				{
					_serialize(out, rsrc_name);
				});

				tcp->send_packet(ClientToMsPacket_MetadataResourcesList, out);
			}

			resources_to_sync.for_each([&](const std::string& rsrc_name)
			{
				// it's possible that by the time the server is handling the resource
				// sync of this current client, the resource doesn't exist anymore (but unlikely)
				// so check if it's valid

				if (const auto rsrc = g_rsrc->get_resource(rsrc_name))
				{
					const auto resource_path = rsrc->get_path();

					serialization_ctx out;

					_serialize(out, rsrc_name);

					std::vector<ResourceFileInfo> files_info;

					rsrc->for_each_client_file([&](const std::string& filename, const FileCtx* ctx)
					{
						const auto file_path = resource_path + filename;
						const auto file_size = static_cast<size_t>(util::fs::file_size(file_path));

						if (file_size >= 0u)
							files_info.emplace_back(filename, util::fs::get_last_write_time(file_path), file_size, ctx->script_type);
					});

					_serialize(out, rsrc->get_total_client_file_size());
					_serialize(out, files_info.size());
					_serialize(out, files_info);

					tcp->send_packet(ClientToMsPacket_SyncResource, out);
				}
			});
		});

		resources_to_sync.clear();
	}
}

void PlayerClient::set_tcp(netcp::tcp_server_client* v)
{
	std::lock_guard lock(tcp_mtx);

	tcp = v;
}

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

	g_rsrc->trigger_event(script::event::ON_PLAYER_JOIN, player);
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
		p.add(target_entity->get_pfx_id());
		p.add(target_entity->get_transform().pack());
		p.add(target_entity->get_hp());
		p.add(target_entity->get_max_hp());

		// serialize into the packet the data from the derived
		// NetObject class, for example, VehicleNetObject etc

		target_entity->serialize_derived(&p);
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