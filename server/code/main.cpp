#define ENET_IMPLEMENTATION
#define ENET_FEATURE_ADDRESS_MAPPING

#include <enet.h>

#include <defs/libs.h>
#include <defs/standard.h>

#include <mp/net.h>

#include <timer/timer.h>

int main()
{
	jc::prof::init("JC:MP Server");
	jc::bug_ripper::init(GetModuleHandle(nullptr));

	util::init();

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

	jc::bug_ripper::destroy();

	return EXIT_SUCCESS;
}