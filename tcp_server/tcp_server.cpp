#include <defs/standard.h>

#include "tcp_server.h"

namespace netcp
{
	// TCP SERVER CLIENT

	tcp_server_client::tcp_server_client(const on_receive_t& on_receive_fn, asio::ip::tcp::socket& socket, CID cid) :
		client_interface(socket, cid, on_receive_fn)
	{
		const auto remote_ep = this->socket.remote_endpoint();

		port = remote_ep.port();
		ip = remote_ep.address().to_string();
		future = std::async(std::launch::async, &tcp_server_client::update, this);
	}

	// TCP SERVER

	tcp_server::tcp_server(uint16_t port) : acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{
		// create a total of 65534 possible cids (1-65534)

		for (CID i = 1ui16; i < MAX_CONNECTIONS; ++i)
			free_cids.insert(free_cids.end(), i);
	}

	tcp_server::~tcp_server() {}

	CID tcp_server::get_free_cid()
	{
		const auto it = free_cids.begin();
		const auto nid = *it;

		free_cids.erase(it);
		used_cids.insert(nid);

		return nid;
	}

	void tcp_server::start()
	{
		accept_connections();

		ctx_thread = std::thread([this]() { ctx.run(); });
	}

	void tcp_server::update()
	{
		while (!GetAsyncKeyState(VK_F7))
		{
			{
				std::lock_guard lock(clients_mtx);

				for (auto it = clients.begin(); it != clients.end();)
				{
					if (const auto& cl = *it; !cl->is_connected())
					{
						free_cid(cl->get_cid());

						it = clients.erase(it);
					}
					else ++it;
				}

				SetConsoleTitleA(std::format("Connections: {} (free cids: {})", clients.size(), free_cids.size()).c_str());
			}

			Sleep(25);
		}
	}

	void tcp_server::free_cid(CID cid)
	{
		auto it = used_cids.find(cid);

		used_cids.erase(it);
		free_cids.insert(cid);
	}

	void tcp_server::accept_connections()
	{
		acceptor.async_accept([this](asio::error_code ec, asio::ip::tcp::socket socket)
		{
			if (!ec)
			{
				std::lock_guard lock(clients_mtx);

				clients.push_back(std::make_shared<tcp_server_client>(on_receive_fn, socket, get_free_cid()));
			}
			else log(RED, "Connection refused '{}'", ec.message());

			accept_connections();
		});
	}
}