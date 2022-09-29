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

		ServerClientType type = ServerClientType_None;

	public:

		tcp_server_client(const on_receive_t& on_receive_fn, asio::ip::tcp::socket& socket, CID cid);

		void set_type(ServerClientType v) { type = v; }

		bool is_client() const { return type == ServerClientType_Client; }
		bool is_server() const { return type == ServerClientType_Server; }

		ServerClientType get_type() const { return type; }

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

		std::unordered_set<CID> free_cids,
								used_cids;

	public:

		tcp_server(uint16_t port);
		~tcp_server();

		CID get_free_cid();

		void start();
		void launch_update_thread();
		void set_on_receive_fn(const on_receive_t& fn) { on_receive_fn = fn; }
		void free_cid(CID cid);
		void accept_connections();
	};
}