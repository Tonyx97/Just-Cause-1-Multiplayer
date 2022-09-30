#pragma once

#include <resource/resource.h>

namespace resource_system
{
	void create_resource_system();
	void destroy_resource_system();
}

class ResourceSystem
{
private:

	std::unordered_map<std::string, Resource*> resources;

public:

	static constexpr auto RESOURCES_FOLDER() { return "resources\\"; }

	bool init();

	void destroy();
	void refresh();

	ResourceVerification verify_resource(const std::string& rsrc_name, ResourceVerificationCtx* ctx);

	ResourceResult start_resource(const std::string& name);
	ResourceResult start_resource(Resource* rsrc);

#ifdef JC_CLIENT
#else
#endif

};

inline std::unique_ptr<ResourceSystem> g_rsrc;