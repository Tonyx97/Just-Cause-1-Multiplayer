#include <defs/standard.h>
#include <defs/libs.h>

#include <sv/sv.h>

#include <timer/timer.h>

int main()
{
	jc::prof::init("JC:MP Server");

	// initialize the server

	g_sv = JC_ALLOC(Server);

	if (!g_sv->init())
		return EXIT_FAILURE;

	// start with the server logic

	while (!GetAsyncKeyState(VK_F7))
	{
		g_sv->tick();

		// check timers
		
		timer::dispatch();
	}

	// clear all resources

	timer::clear_timers();

	// destroy the server

	g_sv->destroy();

	JC_FREE(g_sv);

	return EXIT_SUCCESS;
}