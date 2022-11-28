#include <defs/standard.h>

#include <ms/ms.h>

int main()
{
	jc::prof::init("JC:MP Master Server");
	jc::prof::set_main_thread();
	jc::bug_ripper::init(GetModuleHandle(nullptr));

	BringWindowToTop(GetConsoleWindow());

	MasterServer ms {};
	
	std::string cmd;

	while (cmd != "exit")
	{
		std::cin >> cmd;

		if (cmd == "refresh")
			ms.refresh_client_dll();
	}

	return EXIT_SUCCESS;
}