#pragma once

#include "netcp_interface.h"

#include <thread_system/stl/vector.h>

namespace netcp
{
	class tcp_server_client : public client_interface
	{
	private:

		std::string ip;

		uint16_t port;

	public:

		tcp_server_client(const on_receive_t& on_receive_fn, asio::ip::tcp::socket& socket, CID cid);

		uint16_t get_port() const { return port; }

		const std::string& get_ip() const { return ip; }
	};

	using server_client = std::shared_ptr<tcp_server_client>;

	class tcp_server : public peer_interface
	{
	private:

		asio::ip::tcp::acceptor acceptor;

		CancellableSleep cs;

		jc::thread_safe::vector<server_client> clients;

		std::thread update_thread;

		on_receive_t on_receive_fn = nullptr;
		on_connected_t on_connected_fn = nullptr;
		on_connected_t on_disconnected_fn = nullptr;

		std::unordered_set<CID> free_cids,
								used_cids;

	public:

		tcp_server(uint16_t port);
		~tcp_server();

		CID get_free_cid();

		void start();
		void launch_update_thread();
		void broadcast(uint16_t id, const serialization_ctx& data, client_interface* ignore_ci = nullptr);
		void set_on_receive_fn(const on_receive_t& fn) { on_receive_fn = fn; }
		void set_on_connected_fn(const on_connected_t& fn) { on_connected_fn = fn; }
		void set_on_disconnected_fn(const on_connected_t& fn) { on_disconnected_fn = fn; }
		void free_cid(CID cid);
		void accept_connections();
	};
}