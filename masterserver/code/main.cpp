#include <tcp_server.h>

int main()
{
	BringWindowToTop(GetConsoleWindow());

	netcp::tcp_server sv(50010);

	sv.set_on_receive_fn([](netcp::client_interface* cl, const netcp::packet_header& header, const netcp::packet_data& data)
	{
		//printf_s("[0x%x | DATA-R] id = %i, data size = %i\n", cl->get_cid(), header.id, data.v.size());

		printf_s("data: '%s' %i\n", (char*)data.v.data(), data.v.size());

		std::string nice = "nice good str";

		cl->send_packet(1998, nice.data(), nice.length());

		/*std::ofstream file("file_out.jpg", std::ios::binary);

		file.write((char*)data.v.data(), data.v.size());

		file.close();*/
	});

	sv.start();
	sv.update();

	return std::cin.get();
}