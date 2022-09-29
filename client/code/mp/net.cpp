#include <defs/standard.h>

#include <ports.h>

#include "net.h"
#include "logic.h"

#include <shared_mp/player_client/player_client.h>

#include <tcp_client.h>

bool Net::init(const std::string& ip, const std::string& pw, const std::string& nick)
{
	this->nick = nick;

	// initialize tpc connection first

	tcp = JC_ALLOC(netcp::tcp_client, [](netcp::client_interface* ci, const netcp::packet_header& header, serialization_ctx& data)
	{
		g_net->on_tcp_message(ci, &header, data);
	});

	check(tcp, "Could not initialize TCP connection");
	check(tcp->connect(ip, netcp::CLIENT_TO_SERVER_TCP_PORT), "Could not connect to server via TCP");

	tcp->send_packet(ClientToMsPacket_Password, pw);

	log(YELLOW, "Waiting for password ack...");

	// wait until we received the password ack

	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE
	// IMPLEMENT wait_for IN NETCP INTERFACE

	check(tcp_ctx.wait_for([&]() -> bool { return tcp_ctx.password_ack && tcp_ctx.password_valid; }, 32s), "Invalid password");

	log(YELLOW, "Waiting for default server files ack...");

	// wait until we received all default server files (max of 1024 seconds trying to download lmao)

	check(tcp_ctx.wait_for([&]() -> bool { return tcp_ctx.default_server_files_received; }, 1024s), "Could not receive default server files");

	// initialize enet after the important tcp communication

	enet::init();

	logb(GREEN, "enet initialized");

	if (!(client = enet_host_create(nullptr, 1, 2, 0, 0)))
		return logb(RED, "Could not create enet client");

	// setup channels

	enet::setup_channels();

	// establish connection

	ENetAddress address = { 0 };
	ENetEvent	e		= {};

	enet_address_set_host(&address, ip.c_str());
	address.port = netcp::CLIENT_TO_SERVER_GAME_PORT;

	if (!(peer = enet_host_connect(client, &address, 255, 0)))
		return logb(RED, "No peers available for initiating enet");

	enet_peer_timeout(peer, 0, 0, enet::PEER_TIMEOUT);

#ifdef JC_DBG
	while (!connected && enet_host_service(client, &e, 1000) >= 0)
#else
	while (!connected && enet_host_service(client, &e, 10000) >= 0)
#endif
		if (e.type == ENET_EVENT_TYPE_CONNECT)
			connected = true;

	check(connected, "Error while connecting to the server");

	enet_peer_ping_interval(peer, 2000);

	log(GREEN, "Connected");

	// packet to initialize the client in the server

	send(Packet(PlayerClientPID_Init, ChannelID_PlayerClient, nick));

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

	// close tcp connection

	JC_FREE(tcp);
}

void Net::disconnect()
{
	if (!peer || !connected)
		return;

	if (const auto old_local = std::exchange(local, nullptr))
		remove_player_client(old_local);

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

		send(Packet(PlayerClientPID_Join, ChannelID_PlayerClient));
	}
}

void Net::set_game_load_available()
{
	load_game_available = true;
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

Player* Net::get_localplayer() const
{
	return local ? local->get_player() : nullptr;
}

void Net::on_tcp_message(netcp::client_interface* ci, const netcp::packet_header* header, serialization_ctx& data)
{
	using namespace netcp;

	const auto cl = std::bit_cast<netcp::tcp_client*>(ci);

	switch (header->id)
	{
	case ClientToMsPacket_Password:
	{
		tcp_ctx.password_ack = true;
		tcp_ctx.password_valid = _deserialize<bool>(data);
		tcp_ctx.cs.cancel();

		break;
	}
	case ClientToMsPacket_SyncDefaultFiles:
	{
		const auto default_files_count = _deserialize<size_t>(data);

		if (default_files_count > 0u)
		{
			for (size_t i = 0u; i < default_files_count; ++i)
			{
				const auto target_path = _deserialize<std::string>(data);
				const auto data_size = _deserialize<size_t>(data);

				std::vector<uint8_t> file_data(data_size);

				data.read(file_data.data(), file_data.size());

				util::fs::create_bin_file(target_path, file_data);
			}
		}

		tcp_ctx.default_server_files_received = true;
		tcp_ctx.cs.cancel();

		break;
	}
	default: logt(RED, "[{}] Unknown packet id: {}", CURR_FN, header->id);
	}
}