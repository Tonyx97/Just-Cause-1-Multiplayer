#include <defs/standard.h>

#include "net.h"

#include <game/sys/all.h>

#include <shared_mp/net_handlers/all.h>
#include <shared_mp/player_client/player_client.h>

#include <game/sys/world.h>
#include <game/object/character/character.h>

bool Net::init(const std::string& ip, const std::string& nick)
{
	if (enet_initialize() != 0)
		return logb(RED, "Error initializing enet");

	logb(GREEN, "enet initialized");

	enet::init();

	if (!(client = enet_host_create(nullptr, 1, 2, 0, 0)))
		return logb(RED, "Could not create enet client");

	this->nick = nick;

	// setup channels

	setup_channels();

	// establish connection

	ENetAddress address = { 0 };
	ENetEvent	e		= {};

	enet_address_set_host(&address, ip.c_str());
	address.port = enet::GAME_PORT;

	if (!(peer = enet_host_connect(client, &address, 255, 0)))
		return logb(RED, "No peers available for initiating enet");

	enet_peer_timeout(peer, 0, 0, enet::PEER_TIMEOUT);

	if (enet_host_service(client, &e, 500) <= 0 || e.type != ENET_EVENT_TYPE_CONNECT)
	{
		enet_peer_reset(peer);

		return logb(RED, "Connection failed");
	}

	set_state(PCState_Connecting);

	log(GREEN, "Connected");

	return (connected = true);
}

void Net::disconnect()
{
	if (!peer || !connected)
		return;

	enet_peer_disconnect(peer, 0);

	bool disconnected = false;

	enet::dispatch_packets([&](ENetEvent& e)
	{
		switch (e.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(e.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			disconnected = true;
			break;
		}
	}, 500);

	if (disconnected)
	{
		enet_peer_reset(peer);

		log(YELLOW, "Disconnected");
	}

	if (timed_out)
		log(RED, "Time out");

	remove_player_client(local);

	peer	  = nullptr;
	connected = false;
}

void Net::destroy()
{
	disconnect();

	enet_host_destroy(client);
	enet_deinitialize();

	client = nullptr;
}

void Net::add_local(NID nid)
{
	check(!local, "Localplayer must not exists");

	local = add_player_client(nid);
	local->set_nick(nick);
}

void Net::set_state(uint32_t v, const enet::PacketR* p)
{
	switch (state = v)
	{
	case PCState_Connecting:
	{
		send_reliable<ChannelID_PlayerClient>(PlayerClientPID_State, PCState_Initializing, nick);
		break;
	}
	case PCState_Initializing:
	{
		add_local(p->get_uint());
		send_reliable<ChannelID_PlayerClient>(PlayerClientPID_State, PCState_Initialized);

		log(GREEN, "Init packet received (NID: {:x})", get_local()->get_nid());
		log(GREEN, "Localplayer PC: {} ({})", (void*)get_local(), get_localplayer()->get_nick());
		log(GREEN, "Localplayer: {}", (void*)get_localplayer());

		break;
	}
	case PCState_Initialized: break;
	case PCState_Loaded:
	{
		// ask the server to check for net objects and create the proper instances locally,
		// also make sure we retrieve the static info for players too

		send_reliable<ChannelID_Check>(CheckPID_NetObjects);
		send_reliable<ChannelID_Check>(CheckPID_PlayersStaticInfo);

		// let the server know our game status (either loaded or unloaded)

		send_reliable<ChannelID_PlayerClient>(PlayerClientPID_State, PCState_Loaded);

		break;
	}
	}
}

void Net::setup_channels()
{
	// player client dispatcher

	enet::add_channel_dispatcher(ChannelID_PlayerClient, [&](const enet::PacketR& p)
	{
		switch (auto id = p.get_id())
		{
		case PlayerClientPID_State:			return nh::player_client::state(p);
		case PlayerClientPID_Connect:		return nh::player_client::connect(p);
		case PlayerClientPID_Disconnect:	return nh::player_client::disconnect(p);
		case PlayerClientPID_Nick:			return nh::player_client::nick(p);
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
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return enet::PacketRes_NotUsable;

		switch (auto id = p.get_id())
		{
		case ChatPID_Msg: return nh::chat::msg(p);
		}

		return enet::PacketRes_NotFound;
	});

	// generic packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Generic, [&](const enet::PacketR& p)
	{
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return enet::PacketRes_NotUsable;

		switch (auto id = p.get_id())
		{
		case PlayerPID_SyncSpawn:		return nh::player::sync_spawn(p);
		case PlayerPID_Transform:		return nh::player::transform(p);
		case PlayerPID_SetAnim:			return nh::player::set_anim(p);
		case DayCyclePID_SetTime:		return nh::day_cycle::dispatch(p);
		}

		return enet::PacketRes_NotFound;
	});
}

void Net::tick()
{
	if (!connected || !client)
		return;

	enet::dispatch_packets([this](ENetEvent& e)
	{
		switch (e.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
		{
			enet::call_channel_dispatcher(e);
			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
		{
			timed_out = true;
			[[fallthrough]];
		}
		case ENET_EVENT_TYPE_DISCONNECT:
		{
			disconnect();
			break;
		}
		}
	});

	if (auto local_char = g_world->get_localplayer_character())
		send_reliable(PlayerPID_Transform, local_char->get_transform());
}