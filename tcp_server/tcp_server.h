#pragma once

#include "netcp_interface.h"

namespace netcp
{
	class tcp_server_client : public client_interface
	{
	private:

		std::string ip;

		uint16_t port;

	public:

		tcp_server_client(const on_receive_t& on_receive_fn, asio::ip::tcp::socket& socket, CID cid);
	};

	using server_client = std::shared_ptr<tcp_server_client>;

	class tcp_server : public peer_interface
	{
	private:

		asio::ip::tcp::acceptor acceptor;

		std::vector<server_client> clients;

		std::mutex clients_mtx;

		on_receive_t on_receive_fn = nullptr;

		std::unordered_set<CID> free_cids,
								used_cids;

	public:

		tcp_server(uint16_t port);
		~tcp_server();

		CID get_free_cid();

		void start();
		void update();
		void set_on_receive_fn(const on_receive_t& fn) { on_receive_fn = fn; }
		void free_cid(CID cid);
		void accept_connections();
	};
}