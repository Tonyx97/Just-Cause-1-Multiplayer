#include <defs/standard.h>

#include "ms.h"

#include <defs/json.h>

#include <crypto/sha512.h>

void MasterServer::start_client_sv()
{
	// get dll hash

	refresh_client_dll();

	// get changelog

	refresh_changelog();

	// get news

	refresh_news();

	// setup the client server

	client_sv.set_on_connected_fn([&](netcp::tcp_server_client* cl) { add_client(cl); });
	client_sv.set_on_disconnected_fn([&](netcp::tcp_server_client* cl) { remove_client(cl); });

	client_sv.set_on_receive_fn([&](netcp::client_interface* ci, const netcp::packet_header& header, serialization_ctx& data)
	{
		if (const auto client = ci->get_userdata<Client>())
			client->on_receive(header, data);
	});

	// setup the server server

	server_sv.set_on_connected_fn([&](netcp::tcp_server_client* cl) { add_server(cl); });
	server_sv.set_on_disconnected_fn([&](netcp::tcp_server_client* cl) { remove_server(cl); });

	server_sv.set_on_receive_fn([&](netcp::client_interface* ci, const netcp::packet_header& header, serialization_ctx& data)
	{
		if (const auto server = ci->get_userdata<Server>())
			server->on_receive(header, data);
	});

	// setup the web server

	web_sv.set_on_connected_fn([&](netcp::tcp_server_client* cl) { log(GREEN, "WS connection opened"); });
	web_sv.set_on_disconnected_fn([&](netcp::tcp_server_client* cl) { log(RED, "WS connection closed"); });

	web_sv.set_on_receive_fn([&](netcp::client_interface* ci, const netcp::packet_header& header, serialization_ctx& data)
	{
		log(GREEN, "Received something");
	});

	web_sv.set_on_http_request_fn([&](netcp::client_interface* ci, const std::string& query)
	{
		auto send_response = [&](const std::string& text)
		{
			std::string response;

			response += "HTTP/1.1 200 OK\r\n";
			response += "Host: 51.77.201.205:22504\r\n";
			response += "Connection: Keep-Alive\r\n";
			response += "Keep-Alive: timeout=10, max=10\r\n";
			response += "Access-Control-Request-Method: GET\r\n";
			response += "Access-Control-Request-Headers: cache-control,upgrade-insecure-requests\r\n";
			response += "Sec-Fetch-Mode: cors\r\n";
			response += "Accept-Encoding: gzip, deflate\r\n";
			response += "Accept-Language: ru,en;q=0.9,en-GB;q=0.8,en-US;q=0.7\r\n";
			response += FORMATV("Content-Length: {}\r\n", text.length());
			response += "Content-Type: application/json\r\n";
			response += "\r\n";
			response += text;

			std::error_code ec;

			asio::write(ci->get_socket(), asio::buffer(response), asio::transfer_all(), ec);
		};

		switch (util::hash::JENKINS(query))
		{
		case util::hash::JENKINS("/servers"):
		{
			json j;

			for_each_server([&](netcp::tcp_server_client* cl)
			{
				if (const auto server = cl->get_userdata<Server>(); server && server->is_valid())
				{
					const auto info = server->get_info();

					j.push_back(json
					{
						{ "ip", info.ip },
						{ "name", info.name },
						{ "discord", info.discord },
						{ "gamemode", info.gamemode },
						{ "players", info.players.size() },
						{ "max_players", info.max_players },
						{ "passworded", info.password },
					});
				}
			});

			send_response(j.dump());

			break;
		}
		case util::hash::JENKINS("/changelog"):
		{
			send_response(get_changelog());

			break;
		}
		case util::hash::JENKINS("/news"):
		{
			send_response(get_news());

			break;
		}
		default: log(RED, "Unknown http request: '{}'", query);
		}
	});

	// startup all servers

	client_sv.start();
	client_sv.launch_update_thread();

	server_sv.start();
	server_sv.launch_update_thread();

	web_sv.start();
	web_sv.launch_update_thread();
}

void MasterServer::add_client(netcp::tcp_server_client* cl)
{
	if (!cl)
		return;

	cl->set_userdata(new Client(this, cl));

	clients.push(cl);
}

void MasterServer::add_server(netcp::tcp_server_client* cl)
{
	if (!cl)
		return;

	cl->set_userdata(new Server(this, cl));

	servers.push(cl);

	log(GREEN, "Server connected");
}

void MasterServer::remove_client(netcp::tcp_server_client* cl)
{
	if (!cl)
		return;

	const auto base = cl->get_userdata<Client>();

	check(base, "Attempting to delete an invalid base for client");

	delete base;

	clients.erase(cl);
}

void MasterServer::remove_server(netcp::tcp_server_client* cl)
{
	if (!cl)
		return;

	const auto base = cl->get_userdata<Server>();

	check(base, "Attempting to delete an invalid base for server");

	delete base;

	servers.erase(cl);

	log(RED, "Server disconnected");
}

void MasterServer::refresh_client_dll()
{
#ifndef JC_DBG
	std::lock_guard lock(client_dll_mtx);

	client_dll = util::fs::read_bin_file("jcmp_client.dll");

	check(!client_dll.empty(), "Client DLL does not exist");

	client_dll_hash = crypto::sha512_raw(BITCAST(char*, client_dll.data()), client_dll.size());

	log(GREEN, "Client DLL refreshed: {} bytes", client_dll.size());
#endif
}

void MasterServer::refresh_changelog()
{
#ifndef JC_DBG
	std::lock_guard lock(changelog_mtx);

	changelog = util::fs::read_plain_file("ms_files\\changelog.txt").data();

	log(GREEN, "Changelog refreshed: {} bytes", changelog.size());
#endif
}

void MasterServer::refresh_news()
{
#ifndef JC_DBG
	std::lock_guard lock(news_mtx);

	news = util::fs::read_plain_file("ms_files\\news.txt").data();

	log(GREEN, "News refreshed: {} bytes", news.size());
#endif
}

std::vector<uint8_t> MasterServer::get_client_dll_hash() const
{
	std::lock_guard lock(client_dll_mtx);

	return client_dll_hash;
}

std::vector<uint8_t> MasterServer::get_client_dll() const
{
	std::lock_guard lock(client_dll_mtx);

	return client_dll;
}

std::string MasterServer::get_changelog() const
{
	std::lock_guard lock(changelog_mtx);

	return changelog;
}

std::string MasterServer::get_news() const
{
	std::lock_guard lock(news_mtx);

	return news;
}