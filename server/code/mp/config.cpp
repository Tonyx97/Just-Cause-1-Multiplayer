#include <defs/standard.h>

#include <timer/timer.h>

#include "config.h"
#include "net.h"

#include <tcp_client.h>

#include <shared_mp/player_client/player_client.h>

bool Config::init()
{
	// open server config

	auto config_file = std::ifstream(CONFIG_FILE());
	if (!config_file)
		return false;

	config_file >> server_config;

	bool ok = false;

	if (std::tie(server_info.ip, ok) = get_field<std::string>("ip"); !ok)
		server_info.ip.clear();

	if (std::tie(server_info.name, ok) = get_field<std::string>("server_name"); !ok)
		server_info.name = "Default JC1:MP Server";

	if (std::tie(server_info.discord, ok) = get_field<std::string>("discord"); !ok)
		server_info.discord.clear();

	if (std::tie(server_info.community, ok) = get_field<std::string>("community"); !ok)
		server_info.community.clear();

	if (std::tie(server_info.password, ok) = get_field<std::string>("password"); !ok)
		server_info.ip.clear();

	if (std::tie(server_info.gamemode, ok) = get_field<std::string>("gamemode"); !ok)
		server_info.gamemode.clear();

	if (std::tie(server_info.refresh_rate, ok) = get_field<int>("refresh_rate"); !ok)
		server_info.refresh_rate = 60;

	// initialize masterserver connection

	ms_conn = JC_ALLOC(netcp::tcp_client, [](netcp::client_interface* _cl, const netcp::packet_header& header, serialization_ctx& data)
	{
		switch (header.id)
		{
		case ServerToMsPacket_Verify:
		{
			logt(YELLOW, "Message from masterserver: {}", _deserialize<std::string>(data));
			break;
		}
		}
	});

	check(ms_conn, "Could not create master server connection");
	check(ms_conn->connect("127.0.0.1", netcp::SERVER_TO_MS_PORT), "Could not establish connection to the master server");

	ms_conn->send_packet(SharedMsPacket_Type, netcp::ServerClientType_Server);
	ms_conn->send_packet(ServerToMsPacket_Verify, std::string("todo - pending key"));

	return true;
}

void Config::destroy()
{
	// close masterserver connection

	JC_FREE(ms_conn);
}

void Config::process()
{
	static TimerRaw update_ms_info(2500);

	if (update_ms_info.ready())
	{
		serialization_ctx data;

		_serialize(data,
			server_info.ip,
			server_info.name,
			server_info.discord,
			server_info.community,
			server_info.password,
			server_info.gamemode,
			server_info.refresh_rate);

		int players_count = 0;

		g_net->for_each_player_client([&](NID, PlayerClient* pc)
		{
			_serialize(data, pc->get_nick(), pc->get_roles());
			
			++players_count;
		});

		// send this server's info to the masterserver

		ms_conn->send_packet(ServerToMsPacket_Info, data);
	}
}