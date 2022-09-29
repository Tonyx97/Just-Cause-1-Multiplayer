#include <defs/standard.h>

#include "resource_system.h"

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