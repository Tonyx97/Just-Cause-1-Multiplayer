#include <defs/standard.h>

#include "netcp_interface.h"

namespace netcp
{
	client_interface::~client_interface()
	{
		// cancel all operations being carried out in the update thread

		std::error_code ec;

		socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);

		// wait until all operations are cancelled

		if (future.valid())
			future.wait();
	}

	void client_interface::send_packet(uint16_t id, void* out_data, size_t size)
	{
		// since this function can be called from different threads,
		// lock it
		
		std::lock_guard lock(send_mtx);

		header_out.id = id;
		header_out.size = static_cast<uint32_t>(size);

		data_out.clear();
		_serialize(data_out, header_out);
		data_out.append(std::bit_cast<uint8_t*>(out_data), size);

		std::error_code ec;

		asio::write(socket, asio::buffer(data_out.data.data(), data_out.data.size()), asio::transfer_all(), ec);
	}

	void client_interface::send_packet(uint16_t id)
	{
		// since this function can be called from different threads,
		// lock it

		std::lock_guard lock(send_mtx);

		header_out.id = id;
		header_out.size = 0u;

		data_out.clear();
		_serialize(data_out, header_out);

		std::error_code ec;

		asio::write(socket, asio::buffer(data_out.data.data(), data_out.data.size()), asio::transfer_all(), ec);
	}

	void client_interface::update()
	{
		std::error_code ec;

		while (is_connected())
		{
			if (is_http)
			{
				// read http request

				std::string request;

				const auto bytes_read = asio::read_until(socket, asio::dynamic_buffer(request), "\r\n", ec);

				if (ec) break;

				if (bytes_read > 0)
				{
					std::string line;

					std::stringstream ss(request);

					while (std::getline(ss, line))
					{
						std::smatch sm;

						if (std::regex_search(line, sm, std::regex(R"((GET|OPTIONS)\s(.+)\sHTTP\/(\d+)\.(\d+))")))
						{
							const auto query = sm[2].str();
							const auto ver_major = sm[3].str();
							const auto ver_minor = sm[4].str();

							on_http_request(this, query);
						}
						//else log(RED, "{}", line.c_str());
					}
				}
			}
			else
			{
				// read header

				asio::read(socket, asio::buffer(&header_in, sizeof(header_in)), asio::transfer_exactly(sizeof(header_in)), ec);

				if (ec) break;

				// read data (if size is bigger than 0 ofc)

				if (header_in.size > 0u)
				{
					data_in.resize(header_in.size);

					asio::read(socket, asio::buffer(data_in.data.data(), data_in.data.size()), asio::transfer_exactly(header_in.size), ec);

					if (ec) break;

					data_in.recalculate_begin_end();
				}

				on_receive_fn(this, header_in, data_in);
			}
		}

		if (is_connected())
			socket.close();

		// if there was a waiting operation running then cancel it

		cancel_sleep();
	}

	void client_interface::cancel_sleep()
	{
		cs.cancel();
	}

	// PEER INTERFACE
	
	peer_interface::peer_interface() {}

	peer_interface::~peer_interface()
	{
		ctx.stop();

		if (ctx_thread.joinable())
			ctx_thread.join();
	}
}