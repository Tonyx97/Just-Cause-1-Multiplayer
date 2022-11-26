#include <defs/standard.h>

#include "ms_server.h"

#include <ms/ms.h>

void Server::on_receive(const netcp::packet_header& header, serialization_ctx& data)
{
	switch (header.id)
	{
	case ServerToMsPacket_Verify:
	{
		logt(YELLOW, "Verifying server with key: {}", _deserialize<std::string>(data));

		base->send_packet(ServerToMsPacket_Verify, std::string("verified"));

		break;
	}
	case ServerToMsPacket_Info:
	{
		set_info(Info
		{
			.ip = _deserialize<std::string>(data),
			.name = _deserialize<std::string>(data),
			.discord = _deserialize<std::string>(data),
			.community = _deserialize<std::string>(data),
			.gamemode = _deserialize<std::string>(data),
			.refresh_rate = _deserialize<int>(data),
			.password = _deserialize<bool>(data),
		});

		break;
	}
	default: logt(RED, "Unknown server packet id: {}", header.id);
	}
}

void Server::set_info(Info&& v)
{
	std::lock_guard lock(mtx);

	info = std::move(v);

	if (info.ip.empty())
		info.ip = base->get_ip();

	log(YELLOW, "Server info received {}", info.name);
}

Server::Info Server::get_info() const
{
	std::lock_guard lock(mtx);

	return info;
}