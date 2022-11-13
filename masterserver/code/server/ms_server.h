#pragma once

#include <base/base.h>

class Server : public ServerClient
{
public:

	struct Info
	{
		std::string ip,
			name,
			discord,
			community,
			gamemode;

		int refresh_rate = 0;

		bool password = false;
	};

private:

	mutable std::mutex mtx;

	Info info {};

public:

	Server(MasterServer* ms, netcp::tcp_server_client* base) : ServerClient(ms, base) {}

	void on_receive(const netcp::packet_header& header, serialization_ctx& data);
	void set_info(Info&& v);

	Info get_info() const;
};