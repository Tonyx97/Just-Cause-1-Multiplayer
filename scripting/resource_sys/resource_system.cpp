#include <defs/standard.h>

#include "resource_system.h"

#include <mp/net.h>

#include <resource/resource.h>

namespace resource_system
{
	void create_resource_system()
	{
		check(!g_rsrc, "Resource system must not exist before creating it");

		g_rsrc = std::make_unique<ResourceSystem>();
	}

	void destroy_resource_system()
	{
		check(!!g_rsrc, "Resource system does not exist");

		g_rsrc.reset();
	}
}

bool ResourceSystem::init()
{
#ifdef JC_CLIENT

#else
	const auto startup_resources_list = g_net->get_config().get_info().startup_rsrcs;

	for (const auto& rsrc : startup_resources_list)
	{
		logt(GREEN, "Starting up '{}'...", rsrc);
	}

	// startup the specified resources in settings.json
#endif

	return true;
}

uint8_t ResourceSystem::start_resource(const std::string& name)
{
#ifdef JC_CLIENT
#else
#endif

	return ResourceResult_Ok;
}