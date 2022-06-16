#include <defs/standard.h>

#include "net.h"

#include <game/sys/all.h>

#include <mp/net_handlers/all.h>

bool Net::init(const std::string& ip, const std::string& nick)
{
	if (enet_initialize() != 0)
		return logb(RED, "Error initializing enet");

	logb(GREEN, "enet initialized");

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

	log(GREEN, "Connected");
	log(YELLOW, "Waiting for init packet...");

	enet::PacketW init_p(InitPID_Init);

	init_p.add(nick);
	init_p.send(ChannelID_Init);

	if (!enet::wait_until_packet(InitPID_Init, [](ENetEvent& e)
	{
		enet::PacketR p(e);

		check(p.get_channel() == ChannelID_Init, "Invalid init packet channel");

		log(GREEN, "Init packet received ({} - {})", p.get_str(), p.get_channel());

		// add more stuff to this packet since it's quite important
	})) return logb(RED, "Could not receive the init packet");

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

void Net::setup_channels()
{
	enet::add_channel_dispatcher(ChannelID_Generic, [&](const enet::PacketR& p)
	{
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return enet::PacketRes_NotUsable;

		switch (auto id = p.get_id())
		{
		case DayCyclePID_SetTime: return nh::day_cycle::dispatch(p);
		}

		return enet::PacketRes_NotFound;
	});

	enet::add_channel_dispatcher(ChannelID_Chat, [&](const enet::PacketR& p)
	{
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return enet::PacketRes_NotUsable;

		switch (auto id = p.get_id())
		{
		case ChatPID_ChatMsg: return nh::chat::dispatch(p);
		}

		return enet::PacketRes_NotFound;
	});

	enet::add_channel_dispatcher(ChannelID_Init, [&](const enet::PacketR& p)
	{
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
}