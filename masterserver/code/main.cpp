#include <defs/standard.h>

#include <ms/ms.h>

int main()
{
	jc::prof::init("JC:MP Master Server");
	jc::prof::set_main_thread();
	jc::bug_ripper::init(GetModuleHandle(nullptr));

	BringWindowToTop(GetConsoleWindow());

	MasterServer ms {};

	while (!GetAsyncKeyState(VK_F7)) Sleep(25);

	return EXIT_SUCCESS;
}