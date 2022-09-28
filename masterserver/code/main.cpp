#include <defs/standard.h>

#include <tcp_server.h>
#include <tcp_client.h>

int main()
{
	jc::prof::init("JC:MP Master Server");
	jc::bug_ripper::init(GetModuleHandle(nullptr));

	BringWindowToTop(GetConsoleWindow());

	netcp::tcp_server sv(netcp::SERVER_TO_MS_PORT);

	sv.set_on_receive_fn([](netcp::client_interface* _cl, const netcp::packet_header& header, const Buffer& data)
	{
		using namespace netcp;

		const auto cl = std::bit_cast<netcp::tcp_server_client*>(_cl);

		switch (header.id)
		{
		case SharedMsPacket_Type:
		{
			const auto type = data.get<ServerClientType>();

			cl->set_type(type);

			if (type == ServerClientType_Client)
				log(GREEN, "Client connected ({:x} -> {}:{})", cl->get_cid(), cl->get_ip(), cl->get_port());
			else if (type == ServerClientType_Server)
				log(GREEN, "Server connected ({:x} -> {}:{})", cl->get_cid(), cl->get_ip(), cl->get_port());
			else log(RED, "Invalid type of connection received: {}", uint8_t(type));

			break;
		}
		case ServerToMsPacket_Verify:
		{
			if (cl->is_server())
			{
				log(YELLOW, "Verifying server with key: {}", data.get<std::string>());

				cl->send_packet(ServerToMsPacket_Verify, "verified");
			}

			break;
		}
		case ServerToMsPacket_Info:
		{
			if (cl->is_server())
			{
				log(YELLOW, "info: {}", data.get<std::string>());
			}

			break;
		}
		default: log(RED, "Unknown packet id: {}", header.id);
		}
	});

	sv.start();
	sv.update();

	return EXIT_SUCCESS;
}