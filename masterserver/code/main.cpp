#include <defs/standard.h>

#include <tcp_server.h>
#include <tcp_client.h>

int main()
{
	jc::prof::init("JC:MP Master Server");
	jc::bug_ripper::init(GetModuleHandle(nullptr));

	BringWindowToTop(GetConsoleWindow());

	netcp::tcp_server sv(netcp::SERVER_TO_MS_PORT);

	sv.set_on_receive_fn([](netcp::client_interface* ci, const netcp::packet_header& header, serialization_ctx& data)
	{
		using namespace netcp;

		const auto cl = std::bit_cast<netcp::tcp_server_client*>(ci);

		switch (header.id)
		{
		case SharedMsPacket_Type:
		{
			const auto type = _deserialize<ServerClientType>(data);

			cl->set_type(type);

			if (type == ServerClientType_Client)
				logt(GREEN, "Client connected ({:x} -> {}:{})", cl->get_cid(), cl->get_ip(), cl->get_port());
			else if (type == ServerClientType_Server)
				logt(GREEN, "Server connected ({:x} -> {}:{})", cl->get_cid(), cl->get_ip(), cl->get_port());
			else logt(RED, "Invalid type of connection received: {}", uint8_t(type));

			break;
		}
		case ServerToMsPacket_Verify:
		{
			if (cl->is_server())
			{
				logt(YELLOW, "Verifying server with key: {}", _deserialize<std::string>(data));

				cl->send_packet(ServerToMsPacket_Verify, std::string("verified"));
			}

			break;
		}
		case ServerToMsPacket_Info:
		{
			if (cl->is_server())
			{
				const auto ip = _deserialize<std::string>(data);
				const auto name = _deserialize<std::string>(data);
				const auto discord = _deserialize<std::string>(data);
				const auto community = _deserialize<std::string>(data);
				const auto password = _deserialize<std::string>(data);
				const auto gamemode = _deserialize<std::string>(data);
				const auto refresh_rate = _deserialize<int>(data);

				logt(YELLOW, "info 1: {}", name);
				logt(YELLOW, "info 2: {}", discord);
				logt(YELLOW, "info 3: {}", community);
			}

			break;
		}
		default: logt(RED, "Unknown packet id: {}", header.id);
		}
	});

	sv.start();
	sv.update();

	return EXIT_SUCCESS;
}