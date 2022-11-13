#include <defs/standard.h>

#include "ms.h"

void MasterServer::start_client_sv()
{
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

	// startup both servers

	client_sv.start();
	client_sv.launch_update_thread();

	server_sv.start();
	server_sv.launch_update_thread();
}

void MasterServer::add_client(netcp::tcp_server_client* cl)
{
	if (!cl)
		return;

	cl->set_userdata(new Client(this, cl));

	clients.push(cl);
}

void MasterServer::MasterServer::add_server(netcp::tcp_server_client* cl)
{
	if (!cl)
		return;

	cl->set_userdata(new Server(this, cl));

	servers.push(cl);
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

	clients.erase(cl);
}