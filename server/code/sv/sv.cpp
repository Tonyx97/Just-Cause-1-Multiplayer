#include <defs/standard.h>

#include <timer/timer.h>

#include "sv.h"

#include <net_handlers/all.h>

#include <player_client/player_client.h>

bool Server::init()
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

void Server::destroy()
{
	enet_host_destroy(sv);
	enet_deinitialize();
}

void Server::setup_channels()
{
	// generic packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Generic, [&](const enet::PacketR& p)
	{
		/*switch (auto id = p.get_id())
		{
		}*/

		return enet::PacketRes_NotFound;
	});

	// player client dispatcher

	enet::add_channel_dispatcher(ChannelID_PlayerClient, [&](const enet::PacketR& p)
	{
		switch (auto id = p.get_id())
		{
		case PlayerClientPID_Nick: return nh::player_client::nick(p);
		}

		return enet::PacketRes_NotFound;
	});

	// chat dispatcher

	enet::add_channel_dispatcher(ChannelID_Chat, [&](const enet::PacketR& p)
	{
		switch (auto id = p.get_id())
		{
		case ChatPID_ChatMsg: return nh::chat::msg(p);
		}

		return enet::PacketRes_NotFound;
	});

	// conn init dispatcher

	enet::add_channel_dispatcher(ChannelID_Init, [&](const enet::PacketR& p)
	{
		switch (auto id = p.get_id())
		{
		case InitPID_Init: return nh::init::init(p);
		}

		return enet::PacketRes_NotFound;
	});
}

void Server::tick()
{
	// send all independent packets to all clients
	// such as day cycle time etc
	
	send_global_packets();

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
			add_player_client(e);
			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
		{
			AS_PC(e.peer->data)->set_timed_out();

			[[fallthrough]];
		}
		case ENET_EVENT_TYPE_DISCONNECT:
			remove_player_client(e);
		}
	});

	std::this_thread::sleep_for(std::chrono::microseconds(8333));
}

void Server::send_global_packets()
{
	SetWindowText(GetConsoleWindow(), std::format(L"JC:MP Server ({} players connected)", get_player_client_count()).c_str());

	static auto day_cycle_timer = timer::add_timer(1000, [&]()
	{
		static float test = 0.f;

		//test += 0.05f;

		enet::send_broadcast_reliable(DayCyclePID_SetTime, test);
	});
}