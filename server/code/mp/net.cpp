#include <defs/standard.h>

#include <timer/timer.h>

#include "net.h"

#include <rg/rg.h>

#include <shared_mp/net_handlers/all.h>
#include <shared_mp/player_client/player_client.h>

#include <tcp_server.h>

#include <resource_sys/resource_system.h>

namespace world_rg
{
	void on_create_remove(WorldRg* w, librg_event* e, bool create)
	{
		const auto observer_entity = w->get_event_owner(e);
		const auto visible_entity = w->get_event_entity(e);

		if (!visible_entity || !observer_entity || visible_entity == observer_entity)
			return;

		// we are going to check if observer entity started/stopped seeing the entity
		// and send corresponding sync packets
		// NOTE: this does NOT involve any ownership (only visibility, it's two different things)

		// observer entity must always be a player

		if (const auto observer_player = observer_entity->cast<Player>())
		{
			const auto observer_pc = observer_player->get_client();

			check(observer_pc, "Player has no PlayerClient instance (observer)");

			// handle player-player visibility by updating each other from their game

			if (const auto visible_player = visible_entity->cast<Player>())
				observer_pc->sync_player(visible_player, create);
			else
			{
				// handle player-object visibility by updating the object for
				// the player (observer)

				observer_pc->sync_entity(visible_entity, create);
			}
		}
	}

	void on_create(WorldRg* w, librg_event* e) { on_create_remove(w, e, true); }
	void on_remove(WorldRg* w, librg_event* e) { on_create_remove(w, e, false); }
	void on_update(WorldRg* w, librg_event* e) {}
}

bool Net::init()
{
	logt(YELLOW, "Starting...");

	// initialize enet

	enet::init();

	const auto address = ENetAddress
	{
		.host = ENET_HOST_ANY,
		.port = netcp::CLIENT_TO_SERVER_GAME_PORT
	};

	// setup channels

	enet::setup_channels();

	// create server host

	logt(YELLOW, "Creating host...");

	if (!(sv = enet_host_create(&address, enet::MAX_PLAYERS, ChannelID_Max, 0, 0)))
		return logbwt(RED, "Could not create server host");

	logt(GREEN, "Host created");

	// initialize server config
	
	check(config.init(), "Could not initialize config");

	// initialize server world settings

	check(settings.init(), "Could not initialize world settings");

	logt(GREEN, "Connected to masterserver");

	// initialize tcp server so players can connect and communicate
	// with tcp as well

	tcp_server = JC_ALLOC(netcp::tcp_server, netcp::CLIENT_TO_SERVER_TCP_PORT);

	check(tcp_server, "Could not create tcp connection");
	
	tcp_server->set_on_receive_fn([](netcp::client_interface* ci, const netcp::packet_header& header, serialization_ctx& data)
	{
		g_net->on_client_tcp_message(ci, &header, data);
	});

	tcp_server->set_on_connected_fn([](netcp::tcp_server_client* ci)
	{
		g_net->on_client_tcp_connected(ci);
	});

	tcp_server->set_on_disconnected_fn([](netcp::tcp_server_client* ci)
	{
		g_net->on_client_tcp_disconnected(ci);
	});

	tcp_server->start();
	tcp_server->launch_update_thread();

	// init world

	world_rg = JC_ALLOC(WorldRg,
		i16vec3 { RG_WORLD_CHUNK_COUNT, 1, RG_WORLD_CHUNK_COUNT },
		u16vec3 { RG_WORLD_CHUNK_SIZE, UINT16_MAX, RG_WORLD_CHUNK_SIZE },
		//i16vec3 { 1024, 1, 1024 },
		//u16vec3 { 10u, UINT16_MAX, 10u },
		world_rg::on_create,
		world_rg::on_update,
		world_rg::on_remove);

	logt(GREEN, "Server initialized");

	return true;
}

Net::~Net()
{
	// destroy and clear object list

	clear_object_list();

	// free the world rg

	JC_FREE(world_rg);

	// destroy enet

	enet_host_destroy(sv);
	enet_deinitialize();

	// close the tcp connection

	JC_FREE(tcp_server);

	// destroy settings

	settings.destroy();

	// destroy config

	config.destroy();
}

void Net::send_broadcast_impl(const Packet& p, PlayerClient* ignore_pc)
{
	// create enet packet only once

	p.create();

	// for each player client, send the packet

	for_each_player_client([&](NID, PlayerClient* pc)
	{
		if (pc != ignore_pc)
			pc->send(p);
	});
}

void Net::send_broadcast_joined_impl(const Packet& p, PlayerClient* ignore_pc)
{
	// create enet packet only once

	p.create();

	// for each player client, send the packet

	for_each_joined_player_client([&](NID, PlayerClient* pc)
	{
		if (pc != ignore_pc)
			pc->send(p);
	});
}

void Net::tick()
{
	// update config stuff
	
	config.update();

	// update settings and server environment
	
	settings.update();

	// update the sync of all net objects

	refresh_net_object_sync();

	// sync net object instances creation/destruction and ownership

	sync_net_objects();

	// sync resources across all players

	for_each_player_client([](NID, PlayerClient* pc)
	{
		pc->sync_pending_resources();
	});

	// process packets from the players

	enet::dispatch_packets([&](ENetEvent& e)
	{
		switch (e.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
		{
			enet::call_channel_dispatcher(e);
			break;
		}
		case ENET_EVENT_TYPE_CONNECT:
		{
			e.peer->data = nullptr;

			PlayerClient* timed_out_player = nullptr;

			for_each_player_client([&](NID, PlayerClient* pc)
			{
				if (timed_out_player)
					return;

				if (pc->compare_address(e.peer->address))
					timed_out_player = pc;
			});

			if (timed_out_player)
			{
				log(RED, "A player with the same address exists, destroying the old one...");

				remove_player_client(timed_out_player);
			}

			add_player_client(e.peer);

			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
		{
			AS_PC(e.peer->data)->set_timed_out();

			[[fallthrough]];
		}
		case ENET_EVENT_TYPE_DISCONNECT:
		{
			remove_player_client(AS_PC(e.peer->data));

			e.peer->data = nullptr;
		}
		}
	});

	// update resource system things

	g_rsrc->update();

	// call tick event after all packets from players are received

	g_rsrc->trigger_event(script::event::ON_TICK);

	// debug info
	
	static auto packet_per_sec_printer = timer::add_timer(1000, [&]()
	{
		SetWindowText(GetConsoleWindow(), std::format(L"JC:MP Server ({} players, {} p/s, {} b/s)", get_player_clients_count(), sv->totalReceivedPackets, sv->totalReceivedData).c_str());

		sv->totalReceivedData = 0;
		sv->totalReceivedPackets = 0;
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(1000 / config.get_info().refresh_rate));
}

void Net::refresh_net_object_sync()
{
	static TimerRaw refresh_timer(100);

	if (refresh_timer.ready())
	{
		std::vector<NetObject*> distance_objs;

		for_each_net_object([&](NID nid, NetObject* obj)
		{
			// if it's player, send all the player ownerships to them,
			// otherwise we will decide which entity is going to be
			// used in ownership calculation (by distance etc)

			if (const auto player = obj->cast<Player>())
			{
				if (const auto pc = player->get_client(); pc->is_joined())
					player->send_ownerships();
			}
			else
			{
				switch (const auto sync_type = obj->get_sync_type())
				{
				case SyncType_Global:
				case SyncType_Locked: break; // todojc - ignore global and locked for now
				case SyncType_Distance:
				{
					distance_objs.push_back(obj);

					break;
				}
				default: log(RED, "Unknown sync type: {}", sync_type);
				}
			}
		});

		const float chunks_size = static_cast<float>(world_rg->get_chunk_size().x),
					chunk_size = chunks_size * chunks_size;

		for (auto net_obj : distance_objs)
		{
			const auto& obj_position = net_obj->get_position();

			Player* new_owner = nullptr;

			float lowest_dist = chunk_size;

			for_each_joined_player_client([&](NID nid, PlayerClient* pc)
			{
				const auto player = pc->get_player();

				if (const auto dist = glm::distance2(player->get_position(), obj_position); dist < lowest_dist)
				{
					new_owner = player;
					lowest_dist = dist;
				}
			});

			net_obj->set_owner(new_owner);
		}
	}

	// temporal debug to avoid insane congestion when spamming shit

	static TimerRaw destroy_dead_entities(10000);

	if (destroy_dead_entities.ready())
	{
		std::vector<NetObject*> entities_to_destroy;

		for_each_net_object([&](NID nid, NetObject* obj)
		{
			switch (obj->get_type())
			{
			case NetObject_Vehicle:
			{
				log(RED, "Players in vehicle {:x}: {}", nid, BITCAST(VehicleNetObject*, obj)->get_players_count());

				if (obj->get_hp() > 0.f)
					break;

				[[fallthrough]];
			}
			case NetObject_Damageable:
			{
				if (obj->get_hp() <= 0.f)
					entities_to_destroy.push_back(obj);

				break;
			}
			}
		});

		for (auto object : entities_to_destroy)
			destroy_net_object(object);

		if (entities_to_destroy.size() > 0u)
			log(YELLOW, "{} entities destroyed", entities_to_destroy.size());
	}
}

void Net::sync_net_objects(bool force)
{
	static TimerRaw refresh_timer(250);

	if (force || refresh_timer.ready())
		world_rg->update();
}

void Net::sync_default_files(netcp::tcp_server_client* cl)
{
	serialization_ctx data;

	_serialize(data, config.get_default_files_count());

	config.for_each_default_file([&](const Config::DefaultFile& file)
	{
		if (auto fd = std::ifstream(file.src_path, std::ios::binary))
		{
			_serialize(data, file.dst_path);
			_serialize(data, file.data.size());

			data.append(BITCAST(uint8_t*, file.data.data()), file.data.size());
		}
	});
	
	cl->send_packet(ClientToMsPacket_SyncDefaultFiles, data);
}

void Net::sync_resource(const std::string& rsrc_name)
{
	check_main_thread();

	g_rsrc->exec_with_resource_lock([&]()
	{
		for_each_player_client([&](NID, PlayerClient* pc)
		{
			if (const auto rsrc = g_rsrc->get_resource(rsrc_name))
				pc->add_resource_to_sync(rsrc);
		});
	});
}

void Net::on_client_tcp_connected(netcp::tcp_server_client* ci)
{
	using namespace netcp;

	log(GREEN, "TCP Connection OPENED");
}

void Net::on_client_tcp_disconnected(netcp::tcp_server_client* ci)
{
	using namespace netcp;

	log(RED, "TCP Connection CLOSED");

	// if TCP has an associated PlayerClient then make TCP unavailable

	exec_with_object_lock([&]()
	{
		if (const auto pc = get_valid_player_client(ci->get_userdata<PlayerClient>()))
			pc->set_tcp(nullptr);
	});
}

void Net::on_client_tcp_message(netcp::client_interface* ci, const netcp::packet_header* header, serialization_ctx& data)
{
	using namespace netcp;

	const auto cl = std::bit_cast<netcp::tcp_server_client*>(ci);

	switch (header->id)
	{
	case ClientToMsPacket_Password:
	{
		const auto sv_password = get_config().get_info().password;
		const auto cl_password = _deserialize<std::string>(data);

		if (sv_password.empty() || cl_password == sv_password)
		{
			// send the password ack to the client

			cl->send_packet(ClientToMsPacket_Password, true);

			// since the password is correct, let's also send the packet to sync the
			// default server files
			
			sync_default_files(cl);
		}
		else cl->send_packet(ClientToMsPacket_Password, false);

		break;
	}
	case ClientToMsPacket_StartupSync:
	{
		const auto nid = _deserialize<NID>(data);

		exec_with_object_lock([&]()
		{
			const auto pc = get_player_client_by_nid(nid);

			check(pc, "Invalid player NID {:x} in startup sync", nid);

			// check if the TCP IP is the same as the PlayerClient to 
			// avoid exploits and abuses

			check(pc->get_ip() == cl->get_ip(), "PlayerClient's IP is different from TCP connection, possible exploiter... {} - {}", pc->get_ip(), cl->get_ip());

			log(BLUE, "A player with NID {:x} and CID {:x} wants to sync all resources", nid, cl->get_cid());

			// associate this PlayerClient with the TCP client
			// and viceversa

			cl->set_userdata(pc);
			pc->set_tcp(cl);

			// add all currently active resources to sync queue in PlayerClient

			g_rsrc->for_each_resource([&](const std::string&, Resource* rsrc)
			{
				if (rsrc->is_running())
					pc->add_resource_to_sync(rsrc);
			});
		});

		break;
	}
	case ClientToMsPacket_ResourceFile:
	{
		// send the request resource files to the client who requested it

		const auto rsrc_name = _deserialize<std::string>(data);
		const auto do_complete_sync = _deserialize<bool>(data);
		const auto rsrc_path = ResourceSystem::RESOURCES_FOLDER() + rsrc_name + '\\';

		g_rsrc->exec_with_resource_lock([&]()
		{
			// maybe the resource doesn't exist anymore so let's check it

			if (const auto rsrc = g_rsrc->get_resource(rsrc_name))
			{
				auto send_file = [&](const std::string& filename)
				{
					serialization_ctx out;

					_serialize(out, rsrc_name);

					const auto fullpath = rsrc_path + filename;

					if (const auto file_data = util::fs::read_bin_file(fullpath); !file_data.empty())
					{
						_serialize(out, filename);
						_serialize(out, file_data.size());
						_serialize(out, util::fs::get_last_write_time(fullpath));

						out.append(BITCAST(uint8_t*, file_data.data()), file_data.size());
					}

					cl->send_packet(ClientToMsPacket_ResourceFile, out);
				};

				if (!do_complete_sync)
				{
					// get the file list of the requested files

					if (!data.empty())
					{
						const auto files_list = _deserialize<std::vector<std::string>>(data);

						for (const auto& filename : files_list)
							send_file(filename);
					}
				}
				else
				{
					// since it's a complete sync the client is requesting all client files,
					// we can get them from here (server obv)

					rsrc->for_each_client_file([&](const std::string& filename, const FileCtx* ctx)
					{
						send_file(filename);
					});
				}
			}
		});
		
		break;
	}
	default: logt(RED, "[{}] Unknown packet id: {}", CURR_FN, header->id);
	}
}