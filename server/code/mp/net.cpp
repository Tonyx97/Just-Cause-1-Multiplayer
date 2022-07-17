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
		/*switch (auto id = p.get_id())
		{
		}*/

		return enet::PacketRes_NotFound;
	});

	// generic packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Generic, [&](const enet::Packet& p)
	{
		switch (auto id = p.get_id())
		{
		case PlayerPID_Spawn:						return nh::player::spawn(p);
		case PlayerPID_TickInfo:					return nh::player::tick_info(p);
		case PlayerPID_DynamicInfo:					return nh::player::dynamic_info(p);
		case PlayerPID_StanceAndMovement:			return nh::player::stance_and_movement(p);
		case PlayerPID_Health:						return nh::player::health(p);
		}

		return enet::PacketRes_NotFound;
	});
}

void Net::tick()
{
	SetWindowText(GetConsoleWindow(), std::format(L"JC:MP Server ({} players connected)", get_player_clients_count()).c_str());

	// send all independent packets to all clients
	// such as day cycle time etc
	
	send_global_packets();

	// process packets from the players

	int packets_received_this_tick = 0;

	enet::dispatch_packets([&](ENetEvent& e)
	{
		++packets_received_this_tick;

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

	log(RED, "Packets this tick: {}", packets_received_this_tick);

	std::this_thread::sleep_for(std::chrono::microseconds(8333));
}

void Net::send_global_packets()
{
	static auto day_cycle_timer = timer::add_timer(1000, [&]()
	{
		static float test = 0.f;
		static bool enabled = false;

		//test += 0.05f;

		g_net->send_broadcast_reliable<ChannelID_World>(DayCyclePID_SetTime, enabled, test);
	});
}