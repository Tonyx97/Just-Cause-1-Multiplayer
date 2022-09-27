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
		const auto header_ptr = std::bit_cast<uint8_t*>(&header_out);
		const auto data_ptr = std::bit_cast<uint8_t*>(out_data);

		header_out.id = id;
		header_out.size = static_cast<uint32_t>(size);

		data_out.v.clear();
		data_out.v.insert(data_out.v.end(), header_ptr, header_ptr + sizeof(packet_header));
		data_out.v.insert(data_out.v.end(), data_ptr, data_ptr + size);

		std::error_code ec;

		asio::write(socket, asio::buffer(data_out.v.data(), data_out.v.size()), asio::transfer_all(), ec);
	}

	void client_interface::update()
	{
		std::error_code ec;

		while (is_connected())
		{
			// read header

			asio::read(socket, asio::buffer(&header_in, sizeof(header_in)), asio::transfer_exactly(sizeof(header_in)), ec);

			if (ec) break;

			data_in.v.clear();
			data_in.v.shrink_to_fit();
			data_in.v.resize(header_in.size);

			// read data

			asio::read(socket, asio::buffer(data_in.v.data(), data_in.v.size()), asio::transfer_exactly(header_in.size), ec);

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