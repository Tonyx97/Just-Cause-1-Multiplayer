#include <defs/standard.h>

#include <ms/ms.h>

void Client::on_receive(const netcp::packet_header& header, serialization_ctx& data)
{
	switch (header.id)
	{
	case ClientToMsPacket_ServersInfo:
	{
		std::vector<Server::Info> servers_info;

		serialization_ctx out {};

		ms->for_each_server([&](netcp::tcp_server_client* cl)
		{
			if (const auto server = cl->get_userdata<Server>())
				servers_info.push_back(server->get_info());
		});

		_serialize(out, servers_info);

		base->send_packet(ClientToMsPacket_ServersInfo, out);

		break;
	}
	case ClientToMsPacket_ClientHash:
	{
		base->send_packet(ClientToMsPacket_ClientHash, ms->get_client_dll_hash());

		break;
	}
	case ClientToMsPacket_InjectorHelperHash:
	{
		base->send_packet(ClientToMsPacket_InjectorHelperHash, ms->get_inj_helper_dll_hash());

		break;
	}
	case ClientToMsPacket_DownloadClient:
	{
		base->send_packet(ClientToMsPacket_DownloadClient, ms->get_client_dll());

		break;
	}
	case ClientToMsPacket_DownloadInjectorHelper:
	{
		base->send_packet(ClientToMsPacket_DownloadInjectorHelper, ms->get_inj_helper_dll());
		
		break;
	}
	default: logt(RED, "Unknown client packet id: {}", header.id);
	}
}