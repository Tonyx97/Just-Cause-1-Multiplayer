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

		this->socket.set_option(asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO> { 20000 });

		future = std::async(std::launch::async, &tcp_server_client::update, this);
	}

	// TCP SERVER

	tcp_server::tcp_server(uint16_t port, bool is_http) : port(port), is_http(is_http), acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{
		// create a total of 65534 possible cids (1-65534)

		for (CID i = 1ui16; i < MAX_CONNECTIONS; ++i)
			free_cids.insert(free_cids.end(), i);
	}

	tcp_server::~tcp_server()
	{
		// notify the update thread we are done

		cs.cancel();

		// wait until the update thread terminates

		log(GREEN, "Update thread terminated");

		update_thread.join();
	}

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

	void tcp_server::launch_update_thread()
	{
		update_thread = std::thread([&]()
		{
			while (cs.sleep(1s))
			{
				clients.for_each_it([&](auto it)
				{
					if (const auto cl = *it; !cl->is_connected())
					{
						if (on_disconnected_fn)
							on_disconnected_fn(cl.get());

						free_cid(cl->get_cid());

						return true;
					}

					return false;
				});

				if (GetAsyncKeyState(VK_F2))
					log(YELLOW, "[{}] Connections: {} (free cids: {})", port, clients.size(), free_cids.size());
			}
		});
	}

	void tcp_server::broadcast(uint16_t id, const serialization_ctx& data, client_interface* ignore_ci)
	{
		clients.for_each([&](server_client ci)
		{
			if (ci.get() != ignore_ci)
				ci->send_packet(id, data);
		});
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
				auto ci = std::make_shared<tcp_server_client>(on_receive_fn, socket, get_free_cid());

				if (is_http)
					ci->set_http(on_http_request_fn);

				if (on_connected_fn)
					on_connected_fn(ci.get());

				clients.push(std::move(ci));
			}
			else log(RED, "Connection refused '{}'", ec.message());

			accept_connections();
		});
	}
}