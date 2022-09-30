#define ENET_IMPLEMENTATION
#define ENET_FEATURE_ADDRESS_MAPPING

#include <enet.h>

#include <defs/libs.h>
#include <defs/standard.h>

#include <mp/net.h>

#include <timer/timer.h>

#include <resource_sys/resource_system.h>

int main()
{
	jc::prof::init("JC:MP Server");
	jc::bug_ripper::init(GetModuleHandle(nullptr));

	util::init();

	// initialize the server

	g_net = JC_ALLOC(Net);

	if (!g_net->init())
		return EXIT_FAILURE;

	// create resource system instance

	resource_system::create_resource_system();

	// start with the server logic

	while (!GetAsyncKeyState(VK_F7))
	{
		g_net->tick();

		// check timers
		
		timer::dispatch();
	}

	// destroy resource system

	resource_system::destroy_resource_system();

	// clear all resources

	timer::clear_timers();

	// destroy the server

	JC_FREE(g_net);

	// destroy bug ripper

	jc::bug_ripper::destroy();

	return EXIT_SUCCESS;
}