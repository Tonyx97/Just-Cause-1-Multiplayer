#include <defs/standard.h>

#include "net.h"
#include "logic.h"

#include <game/sys/all.h>

#include <shared_mp/net_handlers/all.h>
#include <shared_mp/player_client/player_client.h>
#include <shared_mp/objs/player.h>

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

	connected = true;

	log(GREEN, "Connected");

	// packet to initialize the client in the server
	
	send_reliable<ChannelID_PlayerClient>(PlayerClientPID_Init, nick);

	// make sure the net system is initialized before continuing

	while (!is_initialized())
		tick();

	return connected;
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

	local	  = nullptr;
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
	local->get_player()->set_local();
}

void Net::set_initialized(bool v)
{
	initialized = v;
}

void Net::set_joined(bool v)
{
	if (!is_initialized())
		return;

	joined = v;

	if (joined)
		send_reliable<ChannelID_PlayerClient>(PlayerClientPID_Join);
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
		case PlayerClientPID_Quit:				return nh::player_client::quit(p);
		case PlayerClientPID_SyncInstances:		return nh::player_client::sync_instances(p);
		case PlayerClientPID_StaticInfo:		return nh::player_client::static_info(p);
		case PlayerClientPID_Nick:				return nh::player_client::nick(p);
		}

		return enet::PacketRes_NotFound;
	});

	// chat dispatcher

	enet::add_channel_dispatcher(ChannelID_Chat, [&](const enet::Packet& p)
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

	enet::add_channel_dispatcher(ChannelID_Generic, [&](const enet::Packet& p)
	{
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return enet::PacketRes_NotUsable;

		switch (auto id = p.get_id())
		{
		case PlayerPID_Spawn:			return nh::player::spawn(p);
		case PlayerPID_TickInfo:		return nh::player::tick_info(p);
		case PlayerPID_DynamicInfo:		return nh::player::dynamic_info(p);
		case PlayerPID_Stance:			return nh::player::stance(p);
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

	jc::mp::logic::on_tick();
}

void Net::update_objects()
{
	jc::mp::logic::on_update_objects();
}