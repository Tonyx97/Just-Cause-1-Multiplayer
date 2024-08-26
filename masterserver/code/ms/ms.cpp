#include <defs/standard.h>

#include "ms.h"

#include <defs/json.h>

#include <crypto/sha512.h>

void MasterServer::start_client_sv()
{
	check(load_config(), "Could not load config");

	// get client dll

	refresh_client_dll();

	// get injector helper

	refresh_inj_helper_dll();

	// get changelog

	//refresh_changelog();

	// get news

	///refresh_news();

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

	// startup all servers

	client_sv.start();
	client_sv.launch_update_thread();

	server_sv.start();
	server_sv.launch_update_thread();

	// launch update thread

	auto update_future = std::async(std::launch::async, &MasterServer::update, this);

	std::string cmd;

	while (cmd != "exit")
	{
		std::cin >> cmd;

		switch (util::hash::JENKINS(cmd))
		{
		case util::hash::JENKINS("r_client"):	refresh_client_dll(); break;
		case util::hash::JENKINS("r_inj"):		refresh_inj_helper_dll(); break;
		case util::hash::JENKINS("r_clog"):		refresh_changelog(); break;
		case util::hash::JENKINS("r_news"):		refresh_news(); break;
		case util::hash::JENKINS("r_all"):
		{
			refresh_client_dll();
			refresh_inj_helper_dll();
			refresh_changelog();
			refresh_news();
			break;
		}
		}
	}

	exiting = true;

	update_future.wait();
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

void MasterServer::refresh_inj_helper_dll()
{
#ifndef JC_DBG
	std::lock_guard lock(inj_helper_dll_mtx);

	inj_helper_dll = util::fs::read_bin_file("dinput8.dll");

	check(!inj_helper_dll.empty(), "Injector Helper DLL does not exist");

	inj_helper_dll_hash = crypto::sha512_raw(BITCAST(char*, inj_helper_dll.data()), inj_helper_dll.size());

	log(GREEN, "Injector Helper DLL refreshed: {} bytes", inj_helper_dll.size());
#endif
}

void MasterServer::refresh_changelog()
{
#ifndef JC_DBG
	std::lock_guard lock(changelog_mtx);

	const std::string name = "changelog.txt";
	const std::string filename = "ms_files\\" + name;

	news = util::fs::read_plain_file(filename).data();

	log(GREEN, "Changelog refreshed: {} bytes", news.size());

	std::filesystem::copy_file(filename, web_path + "rsrc\\" + name, std::filesystem::copy_options::overwrite_existing);
#endif
}

void MasterServer::refresh_news()
{
#ifndef JC_DBG
	std::lock_guard lock(news_mtx);

	const std::string name = "news.txt";
	const std::string filename = "ms_files\\" + name;

	news = util::fs::read_plain_file(filename).data();

	log(GREEN, "News refreshed: {} bytes", news.size());

	std::filesystem::copy_file(filename, web_path + "rsrc\\" + name, std::filesystem::copy_options::overwrite_existing);
#endif
}

void MasterServer::update()
{
	while (!exiting)
	{
		json servers_info;

		for_each_server([&](netcp::tcp_server_client* cl)
		{
			if (const auto server = cl->get_userdata<Server>())
			{
				const auto info = server->get_info();

				servers_info.push_back(json
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

		{
			std::ofstream servers_info_file(web_path + "rsrc\\servers.json", std::ios::trunc);

			servers_info_file << servers_info;
		}

		std::this_thread::sleep_for(1s);
	}
}

bool MasterServer::load_config()
{
	if (!std::filesystem::is_regular_file(CONFIG_FILE()))
		std::ofstream(CONFIG_FILE(), std::ios::trunc);

	try
	{
		auto config_file = std::ifstream(CONFIG_FILE());

		config_file >> config;

		if (!jc_json::get_field(config, "web_path", web_path))
			return false;

		log(GREEN, "Web Path: '{}'", web_path);
	}
	catch (...)
	{
		return logbt(RED, "Could not parse {}", CONFIG_FILE());
	}

	return true;
}

std::vector<uint8_t> MasterServer::get_client_dll_hash() const
{
	std::lock_guard lock(client_dll_mtx);

	return client_dll_hash;
}

std::vector<uint8_t> MasterServer::get_inj_helper_dll_hash() const
{
	std::lock_guard lock(inj_helper_dll_mtx);

	return inj_helper_dll_hash;
}

std::vector<uint8_t> MasterServer::get_client_dll() const
{
	std::lock_guard lock(client_dll_mtx);

	return client_dll;
}

std::vector<uint8_t> MasterServer::get_inj_helper_dll() const
{
	std::lock_guard lock(inj_helper_dll_mtx);

	return inj_helper_dll;
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