#pragma once

#include <base/base.h>

class Client : public ServerClient
{
private:

public:

	Client(MasterServer* ms, netcp::tcp_server_client* base) : ServerClient(ms, base) {}

	void on_receive(const netcp::packet_header& header, serialization_ctx& data);
};