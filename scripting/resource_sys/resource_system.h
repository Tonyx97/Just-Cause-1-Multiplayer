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

#ifdef JC_SERVER
	mutable std::recursive_mutex mtx;
#endif

public:

	static constexpr auto RESOURCES_FOLDER() { return "resources\\"; }

	bool init();

	void destroy();
	void refresh();

	bool is_resource_valid(const std::string& rsrc_name) const;

	ResourceVerification verify_resource(const std::string& rsrc_name, ResourceVerificationCtx* ctx);

	ResourceResult start_resource(const std::string& name);
	ResourceResult stop_resource(const std::string& name);
	ResourceResult restart_resource(const std::string& name);

	Resource* get_resource(const std::string& name) const;

	/**
	* iterate all resources (thread-safe)
	*/
	template <typename Fn>
	void for_each_resource(const Fn& fn)
	{
#ifdef JC_SERVER
		std::lock_guard lock(mtx);
#endif

		for (const auto& [rsrc_name, rsrc] : resources)
			fn(rsrc_name, rsrc);
	}

#ifdef JC_CLIENT
#else
	template <typename Fn>
	void exec_with_resource_lock(const Fn& fn)
	{
		std::lock_guard lock(mtx);

		fn();
	}
#endif

};

inline std::unique_ptr<ResourceSystem> g_rsrc;