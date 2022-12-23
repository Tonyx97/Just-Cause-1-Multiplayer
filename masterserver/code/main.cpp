#include <defs/standard.h>
#include <defs/libs.h>

#include <ms/ms.h>

#include <conncpp.hpp>

int main()
{
	jc::prof::init("JC:MP Master Server");
	jc::prof::set_main_thread();
	jc::bug_ripper::init(GetModuleHandle(nullptr));

	BringWindowToTop(GetConsoleWindow());

	MasterServer ms {};

	return EXIT_SUCCESS;
}