#include <defs/standard.h>
#include <defs/libs.h>

#include <mp/net.h>

#include <timer/timer.h>

int main()
{
	jc::prof::init("JC:MP Server");

	// initialize the server

	g_net = JC_ALLOC(Net);

	if (!g_net->init())
		return EXIT_FAILURE;

	// start with the server logic

	while (!GetAsyncKeyState(VK_F7))
	{
		g_net->tick();

		// check timers
		
		timer::dispatch();
	}

	// clear all resources

	timer::clear_timers();

	// destroy the server

	g_net->destroy();

	JC_FREE(g_net);

	return EXIT_SUCCESS;
}