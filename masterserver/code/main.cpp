#include <defs/standard.h>
#include <tcp_server.h>

int main()
{
	jc::prof::init("JC:MP Master Server");
	jc::bug_ripper::init(GetModuleHandle(nullptr));

	BringWindowToTop(GetConsoleWindow());

	netcp::tcp_server sv(netcp::CLIENT_OR_SERVER_TO_MS_PORT);

	sv.set_on_receive_fn([](netcp::client_interface* cl, const netcp::packet_header& header, const Buffer& data)
	{
		switch (header.id)
		{
		case ServerToMsPacket_Verify:
		{
			log(YELLOW, "Verifying server with key: {}", data.get<std::string>());

			cl->send_packet(ServerToMsPacket_Verify, "verified");

			break;
		}
		default: log(RED, "Unknown packet id: {}", header.id);
		}
	});

	sv.start();
	sv.update();

	return EXIT_SUCCESS;
}