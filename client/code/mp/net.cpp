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

	enet_peer_ping_interval(peer, 2000);

	connected = true;

	log(GREEN, "Connected");

	// packet to initialize the client in the server
	
	send_reliable<ChannelID_PlayerClient>(PlayerClientPID_Init, nick);

	// make sure the net system is initialized before continuing

	while (!is_initialized())
		tick();

	return connected;
}

void Net::pre_destroy()
{
	// destroy the local's player client before clearing the object list

	if (const auto old_local = std::exchange(local, nullptr))
		remove_player_client(old_local);

	// destroy and clear object list

	clear_object_list();
}

void Net::disconnect()
{
	if (!peer || !connected)
		return;

	if (const auto old_local = std::exchange(local, nullptr))
		remove_player_client(old_local);

	enet_peer_disconnect(peer, 0);

	if (peer)
	{
		enet_peer_reset(peer);

		log(YELLOW, "Disconnected");
	}

	if (timed_out)
		log(RED, "Time out");

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
	{
		const auto localplayer = local->get_player();
		const auto local_char = localplayer->get_character();

		// make sure we mark our localplayer as spawned

		localplayer->spawn();

		// let everyone know that we entered the world and spawned

		send_reliable<ChannelID_PlayerClient>(PlayerClientPID_Join, local_char->get_real_hp(), local_char->get_max_hp());
	}
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
		case PlayerClientPID_BasicInfo:			return nh::player_client::basic_info(p);
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

	// world packet dispatcher

	enet::add_channel_dispatcher(ChannelID_World, [&](const enet::Packet& p)
	{
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return enet::PacketRes_NotUsable;

		switch (auto id = p.get_id())
		{
		case WorldPID_SpawnObject:			return nh::world::spawn_object(p);
		case WorldPID_SetTime:				return nh::world::day_time(p);
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
		case PlayerPID_StateSync:				return nh::player::state_sync(p);
		case PlayerPID_Respawn:					return nh::player::respawn(p);
		case PlayerPID_DynamicInfo:				return nh::player::dynamic_info(p);
		case PlayerPID_StanceAndMovement:		return nh::player::stance_and_movement(p);
		case PlayerPID_SetWeapon:				return nh::player::set_weapon(p);
		}

		return enet::PacketRes_NotFound;
	});

	// debug packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Debug, [&](const enet::Packet& p)
	{
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return enet::PacketRes_NotUsable;

		return enet::PacketRes_NotFound;
	});
}

void Net::tick()
{
	if (!connected || !client)
		return;

	// do the net logic now

	jc::mp::logic::on_tick();

	// send queued packets and dispatch the incoming ones

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
}

void Net::update_objects()
{
	jc::mp::logic::on_update_objects();
}