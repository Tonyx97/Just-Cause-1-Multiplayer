#pragma once

#include <resource/resource.h>

namespace resource_system
{
	void create_resource_system();
	void destroy_resource_system();
}

class ScriptTimer;

class ResourceSystem
{
private:

	// resources list
	//
	std::unordered_map<std::string, Resource*> resources;

	// events list
	//
	std::unordered_map<std::string, std::unordered_map<Resource*, std::vector<ScriptEventInfo>>> events;

	// commands list
	//
	std::unordered_map<std::string, std::unordered_map<Resource*, std::vector<ScriptCmdInfo>>> commands;

	// timers list
	//
	std::unordered_map<Resource*, std::unordered_set<ScriptTimer*>> timers;

	mutable std::recursive_mutex mtx;

	std::stack<int> cancelled_events;

	struct
	{
		int id = -1;
		bool cancellable = false;
	} curr_event;

public:

	static constexpr auto RESOURCES_FOLDER() { return "resources\\"; }

	bool init();

	void destroy();
	void refresh();
	void clear_resource(Resource* rsrc);
	void cancel_event();
	void update();

	template <typename... A>
	bool trigger_event(const std::string& event_name, A&&... args)
	{
		// set the current event id to be able to cancel it

		curr_event.cancellable = true;

		++curr_event.id;

		if (auto it = events.find(event_name); it != events.end())
			for (const auto& [rsrc, script_events] : it->second)
				for (const auto& event_info : script_events)
					event_info.fn.call(std::forward<A>(args)...);

		curr_event.cancellable = false;

		// check if the called event was cancelled

		if (!cancelled_events.empty() && cancelled_events.top() == curr_event.id--)
		{
			cancelled_events.pop();

			return false;
		}

		return true;
	}

	bool call_command(const std::string& cmd, const std::vector<std::any>& args);

	bool is_resource_valid(const std::string& rsrc_name) const;
	bool is_timer_valid(Resource* rsrc, ScriptTimer* timer);
	bool trigger_remote_event(const std::string& name, const std::vector<std::any>& va);
	bool trigger_non_remote_event(const std::string& name, const luas::variadic_args& va);
	bool add_event(const std::string& name, luas::lua_fn& fn, Script* script, bool allow_remote_trigger);
	bool remove_event(const std::string& name, Script* script);
	bool add_command(const std::string& name, luas::lua_fn& fn, Script* script);
	bool remove_command(const std::string& name, Script* script);

	ScriptTimer* add_timer(luas::lua_fn& fn, Resource* rsrc, std::vector<std::any>& args, int interval, int times);
	bool remove_timer(ScriptTimer* timer, Resource* rsrc);
	bool kill_timer(ScriptTimer* timer, Resource* rsrc);

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