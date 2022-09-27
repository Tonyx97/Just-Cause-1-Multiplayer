#pragma once

#include "netcp_interface.h"

namespace netcp
{
	class tcp_client : public peer_interface, public client_interface
	{
	private:

	public:

		tcp_client(const on_receive_t& on_receive_fn);
		~tcp_client();

		void connect(const std::string& ip, uint16_t port);
	};
}