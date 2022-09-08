#include <defs/standard.h>

#include <timer/timer.h>

#include "net.h"

#include <rg/rg.h>

#include <shared_mp/net_handlers/all.h>
#include <shared_mp/player_client/player_client.h>

bool Net::init()
{
	if (enet_initialize() != 0)
		return logbwt(RED, "Error occurred while initializing server");

	enet::init();

	const auto address = ENetAddress
	{
		.host = ENET_HOST_ANY,
		.port = enet::GAME_PORT
	};

	// setup channels

	enet::setup_channels();

	// create server host

	if (!(sv = enet_host_create(&address, enet::MAX_PLAYERS, ChannelID_Max, 0, 0)))
		return logbwt(RED, "Could not create server host");

	world_rg = JC_ALLOC(WorldRg, i16vec3 { 32, 32, 1 }, u16vec3 { 500u, 500u, UINT16_MAX },
		[](WorldRg*, librg_event*)
		{
			log(GREEN, "create");
		},
		[](WorldRg*, librg_event*) {},
		[](WorldRg*, librg_event*)
		{
			log(GREEN, "remove");
		});

	logt(GREEN, "Server initialized");

	return true;
}

void Net::destroy()
{
	// destroy and clear object list

	clear_object_list();
	
	// free the world rg

	JC_FREE(world_rg);

	// destroy enet

	enet_host_destroy(sv);
	enet_deinitialize();
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
	// process settings and server environment
	
	settings.process();

	// update the sync of all net objects

	refresh_net_object_sync();

	// sync (from the server) global objects such as players' blips etc

	sync_net_objects();

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

	static auto packet_per_sec_printer = timer::add_timer(1000, [&]()
	{
		//log(RED, "Packets per second: {} | Bytes per second: {}", sv->totalReceivedPackets, sv->totalReceivedData);

		SetWindowText(GetConsoleWindow(), std::format(L"JC:MP Server ({} players, {} p/s, {} b/s)", get_player_clients_count(), sv->totalReceivedPackets, sv->totalReceivedData).c_str());

		sv->totalReceivedData = 0;
		sv->totalReceivedPackets = 0;
	});

	std::this_thread::sleep_for(std::chrono::microseconds(8333));
}

void Net::refresh_net_object_sync()
{
	static TimerRaw refresh_timer(1000);

	if (refresh_timer.ready())
	{
		std::vector<NetObject*> distance_objs;

		for_each_net_object([&](NID nid, NetObject* obj)
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
		});

		for (auto net_obj : distance_objs)
		{
			Player* new_streamer = nullptr;

			float lowest_dist = jc::nums::MAXF;

			for_each_joined_player_client([&](NID nid, PlayerClient* pc)
			{
				const auto player = pc->get_player();

				if (const auto dist = glm::distance2(player->get_position(), net_obj->get_position()); dist < lowest_dist)
				{
					new_streamer = player;
					lowest_dist = dist;
				}
			});

			if (net_obj->get_streamer() != new_streamer)
				net_obj->set_streamer(new_streamer);
		}
	}

	// temporal debug to avoid insane congestion when spamming shit

	static TimerRaw destroy_dead_entities(10000);

	if (destroy_dead_entities.ready())
	{
		std::vector<NetObject*> entities_to_destroy;

		for_each_net_object([&](NID nid, NetObject* obj)
		{
			if (obj->get_type() != NetObject_Player && obj->get_hp() <= 0.f)
				entities_to_destroy.push_back(obj);
			else if (obj->get_type() == NetObject_Vehicle)
				log(RED, "Players in vehicle {:x}: {}", nid, BITCAST(VehicleNetObject*, obj)->get_players_count());
		});

		for (auto object : entities_to_destroy)
			destroy_net_object(object);

		if (entities_to_destroy.size() > 0u)
			log(YELLOW, "{} entities destroyed", entities_to_destroy.size());
	}
}

void Net::sync_net_objects()
{
	static TimerRaw refresh_timer(1000);

	if (refresh_timer.ready())
		world_rg->update();
}