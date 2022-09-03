#include <defs/standard.h>

#include <timer/timer.h>

#include "net.h"

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

	setup_channels();

	// create server host

	if (!(sv = enet_host_create(&address, enet::MAX_PLAYERS, ChannelID_Max, 0, 0)))
		return logbwt(RED, "Could not create server host");

	logt(GREEN, "Server initialized");

	return true;
}

void Net::destroy()
{
	// destroy and clear object list

	clear_object_list();

	// destroy enet

	enet_host_destroy(sv);
	enet_deinitialize();
}

void Net::setup_channels()
{
	// player client dispatcher

	enet::add_channel_dispatcher(ChannelID_PlayerClient, [&](const Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case PlayerClientPID_Init:				return nh::player_client::init(p);
		case PlayerClientPID_Join:				return nh::player_client::join(p);
		case PlayerClientPID_SyncInstances:		return nh::player_client::sync_instances(p);
		case PlayerClientPID_Nick:				return nh::player_client::nick(p);
		}

		return PacketRes_NotFound;
	});

	// chat dispatcher

	enet::add_channel_dispatcher(ChannelID_Chat, [&](const Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case ChatPID_Msg: return nh::chat::msg(p);
		}

		return PacketRes_NotFound;
	});

	// world dispatcher

	enet::add_channel_dispatcher(ChannelID_World, [&](const Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case WorldPID_SetTimeScale:		return nh::world::time_scale(p);
		case WorldPID_SetPunchForce:	return nh::world::punch_force(p);
		case WorldPID_SpawnObject:		return nh::world::spawn_object(p);
		case WorldPID_DestroyObject:	return nh::world::destroy_object(p);
		case WorldPID_SyncObject:		return nh::world::sync_object(p);
		}

		return PacketRes_NotFound;
	});

	// generic packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Generic, [&](const Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case PlayerPID_StateSync:					return nh::player::state_sync(p);
		case PlayerPID_Respawn:						return nh::player::respawn(p);
		case PlayerPID_DynamicInfo:					return nh::player::dynamic_info(p);
		case PlayerPID_StanceAndMovement:			return nh::player::stance_and_movement(p);
		case PlayerPID_SetWeapon:					return nh::player::set_weapon(p);
		case PlayerPID_SetVehicle:					return nh::player::set_vehicle(p);
		case PlayerPID_EnterExitVehicle:			return nh::player::enter_exit_vehicle(p);
		case PlayerPID_VehicleControl:				return nh::vehicle::vehicle_control(p);
		case PlayerPID_VehicleHonk:					return nh::vehicle::vehicle_honk(p);
		case PlayerPID_VehicleEngineState:			return nh::vehicle::vehicle_engine_state(p);
		case PlayerPID_VehicleFire:					return nh::vehicle::vehicle_fire(p);
		case PlayerPID_VehicleMountedGunFire:		return nh::vehicle::vehicle_mounted_gun_fire(p);
		}

		return PacketRes_NotFound;
	});

	// generic packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Debug, [&](const Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case DbgPID_SetTime:		return nh::dbg::set_time(p);
		}

		return PacketRes_NotFound;
	});
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
	static TimerRaw refresh_timer(enet::TICKS_MS);

	if (refresh_timer.ready())
	{
	}
}