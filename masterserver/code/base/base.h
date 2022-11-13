#pragma once

#include <tcp_server.h>
#include <tcp_client.h>

class MasterServer;

class ServerClient
{
protected:

	netcp::tcp_server_client* base = nullptr;

	MasterServer* ms = nullptr;

public:

	ServerClient(MasterServer* ms, netcp::tcp_server_client* base) : ms(ms), base(base)
	{
	}

	virtual ~ServerClient() = default;
};