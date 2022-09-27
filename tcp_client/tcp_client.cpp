#include "tcp_client.h"

namespace netcp
{
	tcp_client::tcp_client(const on_receive_t& on_receive_fn) : client_interface(ctx, on_receive_fn) {}
	tcp_client::~tcp_client() {}

	void tcp_client::connect(const std::string& ip, uint16_t port)
	{
		asio::ip::tcp::resolver resolver(ctx);
		asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(ip, std::to_string(port));

		asio::async_connect(socket, endpoints, [this](asio::error_code ec, asio::ip::tcp::endpoint ep)
		{
			if (!ec)
				printf_s("Connected\n");
			else printf_s("Could not connect %s\n", ec.message().c_str());
		});

		ctx.run();

		future = std::async(std::launch::async, &tcp_client::update, this);
	}
}