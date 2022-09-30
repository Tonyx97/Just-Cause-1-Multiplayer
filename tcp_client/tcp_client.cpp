#include <defs/standard.h>

#include "tcp_client.h"

namespace netcp
{
	tcp_client::tcp_client(const on_receive_t& on_receive_fn) : client_interface(ctx, on_receive_fn) {}
	tcp_client::~tcp_client() {}

	bool tcp_client::connect(const std::string& ip, uint16_t port)
	{
		asio::ip::tcp::resolver resolver(ctx);
		asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(ip, std::to_string(port));
		
		std::error_code ec;

		asio::connect(socket, endpoints, ec);

		if (ec)
			return false;

		future = std::async(std::launch::async, &tcp_client::update, this);

		return true;
	}
}