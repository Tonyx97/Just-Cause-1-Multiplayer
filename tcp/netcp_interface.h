#pragma once

#include <unordered_set>

#define _WIN32_WINNT 0x0A00

#include <asio.hpp>

#include "ports.h"
#include "packets.h"

#include <utils/buffer.h>

namespace netcp
{
	using CID = uint16_t;

	static constexpr uint16_t MAX_CONNECTIONS = (1 << 16) - 1;
	static constexpr CID INVALID_CID = 0u;

#pragma pack(push, 1)
	struct packet_header
	{
		uint16_t id = 0ui16;
		uint32_t size = 0u;
	};
#pragma pack(pop)

	using on_receive_t = std::function<void(class client_interface*, const packet_header& header, const Buffer& data)>;

	class client_interface
	{
	protected:

		std::future<void> future;

		asio::ip::tcp::socket socket;

		on_receive_t on_receive_fn = nullptr;

		packet_header header_in {},
					  header_out {};

		Buffer data_in {},
			   data_out {};

		CID cid = INVALID_CID;

	public:

		client_interface(
			asio::io_context& ctx,
			const on_receive_t& receive_fn) :
			socket(ctx),
			on_receive_fn(receive_fn) {}

		client_interface(
			asio::ip::tcp::socket& socket,
			CID cid,
			const on_receive_t& receive_fn) :
			socket(std::move(socket)),
			cid(cid),
			on_receive_fn(receive_fn) {}

		~client_interface();

		void send_packet(uint16_t id, void* out_data, size_t size);
		void update();

		template <typename T>
		void send_packet(uint16_t id, const T& out_data)
		{
			Buffer data;

			data.add(out_data);

			send_packet(id, std::bit_cast<void*>(data.data.data()), data.data.size());
		}

		bool is_connected() const { return socket.is_open(); }

		CID get_cid() const { return cid; }

		const asio::ip::tcp::socket& get_socket() const { return socket; }
	};

	class peer_interface
	{
	protected:

		asio::io_context ctx;

		std::thread ctx_thread;

	public:

		peer_interface();
		~peer_interface();
	};
}