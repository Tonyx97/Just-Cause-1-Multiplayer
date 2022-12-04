#pragma once

#define _WIN32_WINNT 0x0A00

#include <asio.hpp>

#include "ports.h"
#include "packets.h"

#include <serializer/serializer.h>

#include <thread_system/cancellable_sleep.h>

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

	using on_receive_t = std::function<void(class client_interface*, const packet_header& header, serialization_ctx& data)>;
	using on_http_request_t = std::function<void(class client_interface*, const std::string& header)>;
	using on_connected_t = std::function<void(class tcp_server_client*)>;

	class client_interface
	{
	protected:

		std::future<void> future;

		asio::ip::tcp::socket socket;

		CancellableSleep cs;

		std::mutex send_mtx;

		on_receive_t on_receive_fn = nullptr;

		on_http_request_t on_http_request = nullptr;

		packet_header header_in {},
					  header_out {};

		serialization_ctx data_in {},
						  data_out {};

		std::atomic<void*> userdata = nullptr;

		bool is_http = false;

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
		void send_packet(uint16_t id);
		void update();
		void cancel_sleep();
		void set_userdata(void* v) { userdata = v; }
		void set_http(const on_http_request_t& fn)
		{
			on_http_request = fn;
			is_http = true;
		}

		template <typename T>
		void send_packet(uint16_t id, const T& out_data) requires(!std::is_same_v<T, serialization_ctx>)
		{
			serialization_ctx data;

			_serialize(data, out_data);

			send_packet(id, std::bit_cast<void*>(data.data.data()), data.data.size());
		}

		void send_packet(uint16_t id, const serialization_ctx& data)
		{
			send_packet(id, std::bit_cast<void*>(data.data.data()), data.data.size());
		}

		/**
		* if connection is closed or lost, the sleep will be cancelled
		* and returning properly without any halt
		*/
		template <typename T, typename Fn>
		bool wait_for(T timeout, const Fn& fn)
		{
			cs.sleep(timeout);

			return fn();
		}
		
		bool is_connected() const { return socket.is_open(); }

		CID get_cid() const { return cid; }

		asio::ip::tcp::socket& get_socket() { return socket; }

		template <typename T>
		T* get_userdata() const { return std::bit_cast<T*>(userdata.load()); }
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