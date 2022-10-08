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

	mutable std::recursive_mutex mtx;

public:

	static constexpr auto RESOURCES_FOLDER() { return "resources\\"; }

	bool init();

	void destroy();
	void refresh();

	bool is_resource_valid(const std::string& rsrc_name) const;

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
		std::lock_guard lock(mtx);

		for (const auto& [rsrc_name, rsrc] : resources)
			fn(rsrc_name, rsrc);
	}

	template <typename Fn>
	bool for_each_resource_ret(const Fn& fn)
	{
		std::lock_guard lock(mtx);

		for (const auto& [rsrc_name, rsrc] : resources)
			if (!fn(rsrc_name, rsrc))
				return false;

		return true;
	}

	template <typename Fn>
	void exec_with_resource_lock(const Fn& fn)
	{
		std::lock_guard lock(mtx);

		fn();
	}

#ifdef JC_CLIENT
	Resource* create_resource(const std::string& name);
#else
	ResourceVerification verify_resource(const std::string& rsrc_name, ResourceVerificationCtx* ctx);
	ResourceVerification read_meta(const std::string& rsrc_name, ResourceVerificationCtx* ctx);
#endif
};

inline std::unique_ptr<ResourceSystem> g_rsrc;