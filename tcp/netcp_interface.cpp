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
		header_out.id = id;
		header_out.size = static_cast<uint32_t>(size);

		data_out.clear();
		data_out.add(header_out);
		data_out.insert(out_data, size);

		std::error_code ec;

		asio::write(socket, asio::buffer(data_out.data.data(), data_out.data.size()), asio::transfer_all(), ec);
	}

	void client_interface::update()
	{
		std::error_code ec;

		while (is_connected())
		{
			// read header

			asio::read(socket, asio::buffer(&header_in, sizeof(header_in)), asio::transfer_exactly(sizeof(header_in)), ec);

			if (ec) break;

			data_in.clear();
			data_in.data.resize(header_in.size);

			// read data

			asio::read(socket, asio::buffer(data_in.data.data(), data_in.data.size()), asio::transfer_exactly(header_in.size), ec);

			if (ec) break;

			on_receive_fn(this, header_in, data_in);
		}

		if (is_connected())
			socket.close();
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