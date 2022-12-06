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

		switch (util::hash::JENKINS(cmd))
		{
		case util::hash::JENKINS("r_client"):	ms.refresh_client_dll(); break;
		case util::hash::JENKINS("r_clog"):		ms.refresh_changelog(); break;
		}
	}

	return EXIT_SUCCESS;
}