#pragma once

namespace resource_system
{
	void create_resource_system();
	void destroy_resource_system();
}

class ResourceSystem
{
private:
public:
};

inline std::unique_ptr<ResourceSystem> g_rsrc;