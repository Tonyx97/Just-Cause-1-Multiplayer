#pragma once

#include <base/base.h>
#include <client/ms_client.h>
#include <server/ms_server.h>

class MasterServer
{
private:

	netcp::tcp_server client_sv,
					  server_sv;

	std::mutex mtx;

	jc::thread_safe::vector<netcp::tcp_server_client*> clients,
													   servers;

public:

	MasterServer() : client_sv(netcp::CLIENT_TO_MS_PORT), server_sv(netcp::SERVER_TO_MS_PORT)
	{
		start_client_sv();
	}

	void start_client_sv();
	void add_client(netcp::tcp_server_client* cl);
	void add_server(netcp::tcp_server_client* cl);
	void remove_client(netcp::tcp_server_client* cl);
	void remove_server(netcp::tcp_server_client* cl);

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
};