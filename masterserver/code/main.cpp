#include <defs/standard.h>
#include <tcp_server.h>

int main()
{
	BringWindowToTop(GetConsoleWindow());

	netcp::tcp_server sv(netcp::CLIENT_OR_SERVER_TO_MS_PORT);

	sv.set_on_receive_fn([](netcp::client_interface* cl, const netcp::packet_header& header, const netcp::packet_data& data)
	{
		printf_s("data: '%s' %i\n", (char*)data.v.data(), data.v.size());

	});

	sv.start();
	sv.update();

	return std::cin.get();
}