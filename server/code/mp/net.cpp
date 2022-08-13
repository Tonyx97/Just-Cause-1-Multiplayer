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

	enet::add_channel_dispatcher(ChannelID_PlayerClient, [&](const enet::Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case PlayerClientPID_Init:				return nh::player_client::init(p);
		case PlayerClientPID_Join:				return nh::player_client::join(p);
		case PlayerClientPID_SyncInstances:		return nh::player_client::sync_instances(p);
		case PlayerClientPID_Nick:				return nh::player_client::nick(p);
		}

		return enet::PacketRes_NotFound;
	});

	// chat dispatcher

	enet::add_channel_dispatcher(ChannelID_Chat, [&](const enet::Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case ChatPID_Msg: return nh::chat::msg(p);
		}

		return enet::PacketRes_NotFound;
	});

	// world dispatcher

	enet::add_channel_dispatcher(ChannelID_World, [&](const enet::Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case WorldPID_SpawnObject:		return nh::world::spawn_object(p);
		case WorldPID_DestroyObject:	return nh::world::destroy_object(p);
		case WorldPID_SyncObject:		return nh::world::sync_object(p);
		}

		return enet::PacketRes_NotFound;
	});

	// generic packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Generic, [&](const enet::Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case PlayerPID_StateSync:					return nh::player::state_sync(p);
		case PlayerPID_Respawn:						return nh::player::respawn(p);
		case PlayerPID_DynamicInfo:					return nh::player::dynamic_info(p);
		case PlayerPID_StanceAndMovement:			return nh::player::stance_and_movement(p);
		case PlayerPID_SetWeapon:					return nh::player::set_weapon(p);
		}

		return enet::PacketRes_NotFound;
	});

	// generic packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Debug, [&](const enet::Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case DbgPID_SetTime:		return nh::dbg::set_time(p);
		}

		return enet::PacketRes_NotFound;
	});
}

void Net::tick()
{
	SetWindowText(GetConsoleWindow(), std::format(L"JC:MP Server ({} players connected)", get_player_clients_count()).c_str());

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
		log(RED, "Packets per second: {} | Bytes per second: {}", sv->totalReceivedPackets, sv->totalReceivedData);

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

		log(RED, "checking distance objects: {}", distance_objs.size());

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
			{
				log(GREEN, "New streamer for this net object {:x} -> {:x}", ptr(net_obj), ptr(new_streamer));

				net_obj->set_streamer(new_streamer);
			}
		}
	}
}

void Net::sync_net_objects()
{
	static TimerRaw refresh_timer(enet::TICKS_MS);

	if (refresh_timer.ready())
	{
	}
}