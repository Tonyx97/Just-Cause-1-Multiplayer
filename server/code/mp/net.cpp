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
	enet_host_destroy(sv);
	enet_deinitialize();
}

void Net::setup_channels()
{
	// player client dispatcher

	enet::add_channel_dispatcher(ChannelID_PlayerClient, [&](const enet::PacketR& p)
	{
		switch (auto id = p.get_id())
		{
		case PlayerClientPID_State:		return nh::player_client::state(p);
		case PlayerClientPID_Nick:		return nh::player_client::nick(p);
		}

		return enet::PacketRes_NotFound;
	});

	// check dispatcher

	enet::add_channel_dispatcher(ChannelID_Check, [&](const enet::PacketR& p)
	{
		switch (auto id = p.get_id())
		{
		case CheckPID_NetObjects:			return nh::check::net_objects(p);
		case CheckPID_PlayersStaticInfo:	return nh::check::players_static_info(p);
		}

		return enet::PacketRes_NotFound;
	});

	// chat dispatcher

	enet::add_channel_dispatcher(ChannelID_Chat, [&](const enet::PacketR& p)
	{
		switch (auto id = p.get_id())
		{
		case ChatPID_Msg: return nh::chat::msg(p);
		}

		return enet::PacketRes_NotFound;
	});

	// generic packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Generic, [&](const enet::PacketR& p)
	{
		switch (auto id = p.get_id())
		{
		case PlayerPID_SyncSpawn: return nh::player::sync_spawn(p);
		case PlayerPID_Transform: return nh::player::transform(p);
		case PlayerPID_SetAnim: return nh::player::set_anim(p);
		}

		return enet::PacketRes_NotFound;
	});
}

void Net::tick()
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

	std::this_thread::sleep_for(std::chrono::microseconds(8333));
}

void Net::send_global_packets()
{
	SetWindowText(GetConsoleWindow(), std::format(L"JC:MP Server ({} players connected)", get_player_clients_count()).c_str());

	static auto day_cycle_timer = timer::add_timer(1000, [&]()
	{
		static float test = 0.f;

		//test += 0.05f;

		g_net->send_broadcast_reliable(DayCyclePID_SetTime, test);
	});
}

void Net::sync_net_objects_instances_for_player(PlayerClient* pc)
{
	enet::PacketW p(CheckPID_NetObjects);

	int count = 0;

	g_net->for_each_net_object([&](NID, NetObject* obj)
	{
		p.add(obj);
		
		++count;
	});

	p.add_begin(count);
	p.ready();

	pc->send<ChannelID_Check>(p);
}

void Net::sync_players_static_info()
{
	enet::PacketW p(CheckPID_PlayersStaticInfo);

	int count = 0;

	for_each_player_client([&](NID, PlayerClient* pc)
	{
		if (const auto player = pc->get_player(); pc->is_initialized() || pc->is_loaded())
		{
			const auto& static_info = player->get_static_info();

			PacketCheck_PlayerStaticInfo info;

			info.nick = static_info.nick;
			info.skin = static_info.skin;

			log(PURPLE, "syncing player static info for {:x} ({})", player->get_nid(), player->get_nick());

			p.add(player);
			p.add(info);

			++count;
		}
	});

	p.add_begin(count);

	send_broadcast_reliable<ChannelID_Check>(p);
}

void Net::sync_players_spawn()
{
	for_each_player_client([&](NID, PlayerClient* pc)
	{
		if (const auto player = pc->get_player())
			player->sync_spawn();
	});
}