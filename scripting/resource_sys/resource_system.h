#pragma once

namespace resource_system
{
	void create_resource_system();
	void destroy_resource_system();
}

class Resource;

class ResourceSystem
{
private:

	std::unordered_map<std::string, Resource*> resources;

public:

	bool init();

	uint8_t start_resource(const std::string& name);

#ifdef JC_CLIENT
#else
#endif

};

inline std::unique_ptr<ResourceSystem> g_rsrc;