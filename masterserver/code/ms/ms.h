#pragma once

#include <base/base.h>
#include <client/ms_client.h>
#include <server/ms_server.h>

class MasterServer
{
private:

	netcp::tcp_server client_sv,
					  server_sv,
					  web_sv;

	mutable std::mutex clients_mtx;
	mutable std::mutex servers_mtx;
	mutable std::mutex client_dll_mtx;
	mutable std::mutex changelog_mtx;
	mutable std::mutex news_mtx;

	jc::thread_safe::vector<netcp::tcp_server_client*> clients,
													   servers,
													   web_clients;

	std::vector<uint8_t> client_dll,
						 client_dll_hash;

	std::string changelog;
	std::string news;

public:

	MasterServer() :
		client_sv(netcp::CLIENT_TO_MS_PORT),
		server_sv(netcp::SERVER_TO_MS_PORT),
		web_sv(netcp::WEBSITE_TO_MS_PORT, true)
	{
		start_client_sv();
	}

	void start_client_sv();
	void add_client(netcp::tcp_server_client* cl);
	void add_server(netcp::tcp_server_client* cl);
	void remove_client(netcp::tcp_server_client* cl);
	void remove_server(netcp::tcp_server_client* cl);
	void refresh_client_dll();
	void refresh_changelog();
	void refresh_news();

	std::vector<uint8_t> get_client_dll_hash() const;
	std::vector<uint8_t> get_client_dll() const;

	std::string get_changelog() const;
	std::string get_news() const;

	template <typename Fn>
	void for_each_client(const Fn& fn)
	{
		clients.for_each([&](auto client) { fn(client); });
	}

	template <typename Fn>
	void for_each_server(const Fn& fn)
	{
		servers.for_each([&](auto server) { fn(server); });
	}

	size_t get_clients_count() const
	{
		return clients.size();
	}

	size_t get_servers_count() const
	{
		return servers.size();
	}
};