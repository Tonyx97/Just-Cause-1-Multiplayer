#include <defs/standard.h>
#include <defs/json.h>

#include "resource_system.h"

#include <mp/net.h>

#include <resource/resource.h>

#include <luas.h>

#include <mp/net.h>

namespace resource_system
{
	void create_resource_system()
	{
		check(!g_rsrc, "Resource system must not exist before creating it");

		g_rsrc = std::make_unique<ResourceSystem>();

		fatal_error_callback = [](const char*) { return 0; };
		error_callback = [](const char* err) { log(RED, "{}", err); return 0; };
	}

	void destroy_resource_system()
	{
		check(!!g_rsrc, "Resource system does not exist");

		g_rsrc.reset();
	}
}

bool ResourceSystem::init()
{
	util::fs::create_directory(RESOURCES_FOLDER());

	// create our own stack pusher (used when triggering remote events mostly
	// aka pushing a std::vector<std::any>)
	
	custom_stack_pusher = [](const luas::state& state, const std::any& v)
	{
		// kinda trash but it is what it is

		const auto& type = v.type();

		if (type == typeid(int))				state.push(std::any_cast<int>(v));
		else if (type == typeid(double))		state.push(std::any_cast<double>(v));
		else if (type == typeid(std::string))	state.push(std::any_cast<std::string>(v));
		else if (type == typeid(bool))			state.push(std::any_cast<bool>(v));
		else if (type == typeid(float))			state.push(std::any_cast<float>(v));
		else if (type == typeid(NetObject*))	state.push(std::any_cast<NetObject*>(v));
		else if (type == typeid(uint32_t))		state.push(std::any_cast<uint32_t>(v));
		else if (type == typeid(uint8_t))		state.push(std::any_cast<uint8_t>(v));
		else if (type == typeid(int8_t))		state.push(std::any_cast<int8_t>(v));
		else if (type == typeid(uint16_t))		state.push(std::any_cast<uint16_t>(v));
		else if (type == typeid(int16_t))		state.push(std::any_cast<int16_t>(v));
		else if (type == typeid(uint64_t))		state.push(std::any_cast<uint64_t>(v));
		else if (type == typeid(int64_t))		state.push(std::any_cast<int64_t>(v));

		return 1;
	};

#ifdef JC_CLIENT
#else
	// verify and add all resources we can find

	refresh();

	// startup the resources that were specified in settings.json

	const auto& startup_resources_list = g_net->get_config().get_info().startup_rsrcs;

	for (const auto& rsrc_name : startup_resources_list)
		start_resource(rsrc_name);
#endif

	return true;
}

void ResourceSystem::destroy()
{
	std::lock_guard lock(mtx);

	for (const auto& [name, rsrc] : resources)
	{
		stop_resource(name);

		JC_FREE(rsrc);
	}

	resources.clear();
}

void ResourceSystem::refresh()
{
	std::lock_guard lock(mtx);

#ifdef JC_CLIENT
#else
	std::unordered_map<std::string, ResourceVerificationCtx> updated_list;

	// grab new list of valid resources first

	for (const auto& p : std::filesystem::directory_iterator(RESOURCES_FOLDER()))
	{
		const auto path = p.path();
		const auto rsrc_name = path.stem().string();

		ResourceVerificationCtx ctx;

		if (verify_resource(rsrc_name, &ctx) == ResourceVerification_Ok)
			updated_list.insert({ rsrc_name, std::move(ctx) });
	}

	// add new resources

	for (const auto& [rsrc_name, ctx] : updated_list)
	{
		// ignore already created ones

		if (!is_resource_valid(rsrc_name))
		{
			const auto resource = JC_ALLOC(Resource, rsrc_name, ctx);

			resources.insert({ rsrc_name, resource });

			logt(YELLOW, "Resource '{}' created", rsrc_name);
		}
	}

	// remove old resources

	util::container::map::remove_free_any_if(resources, [&](const std::string& rsrc_name, Resource* rsrc)
	{
		if (!updated_list.contains(rsrc_name))
		{
			logt(RED, "Resource '{}' no longer exists, it's destroyed now", rsrc_name);

			return true;
		}

		return false;
	});
#endif
}

void ResourceSystem::clear_resource_events(Resource* rsrc)
{
	for (auto& [_, rsrc_events] : events)
		rsrc_events.erase(rsrc);
}

void ResourceSystem::cancel_event()
{
	if (curr_event.cancellable)
		cancelled_events.push(curr_event.id);
}

bool ResourceSystem::is_resource_valid(const std::string& rsrc_name) const
{
	std::lock_guard lock(mtx);

	return resources.contains(rsrc_name);
}

bool ResourceSystem::trigger_remote_event(const std::string& name, const std::vector<std::any>& va)
{
	if (auto it = events.find(name); it != events.end())
		for (const auto& [rsrc, script_events] : it->second)
			for (const auto& event_info : script_events)
				if (event_info.allow_remote_trigger)
					event_info.fn.call(va);

	return true;
}

bool ResourceSystem::trigger_non_remote_event(const std::string& name, const luas::variadic_args& va)
{
	if (auto it = events.find(name); it != events.end())
		for (const auto& [rsrc, script_events] : it->second)
			for (const auto& event_info : script_events)
				event_info.fn.call(va);

	return true;
}

bool ResourceSystem::add_event(const std::string& name, luas::lua_fn& fn, Script* script, bool allow_remote_trigger)
{
	if (!script)
		return false;

	auto rsrc_owner = script->get_owner();
	if (!rsrc_owner)
		return false;

	ScriptEventInfo new_entry =
	{
		.name = name,
		.script = script,
		.fn = std::move(fn),
		.allow_remote_trigger = allow_remote_trigger
	};

	// find all resources that register this event

	if (auto it_event = events.find(name); it_event != events.end())
	{
		auto& rsrc_event_info = it_event->second;

		// find the script's resource in the event resource list
		
		if (auto it_rsrc_event = rsrc_event_info.find(rsrc_owner); it_rsrc_event != rsrc_event_info.end())
		{
			auto& rsrc_script_events = it_rsrc_event->second;

			// do not register the event in the same script
			
			for (const auto& event_info : rsrc_script_events)
				if (event_info.script == script)
					return false;

			rsrc_script_events.push_back(std::move(new_entry));
		}
		else rsrc_event_info[rsrc_owner].push_back(std::move(new_entry));
	}
	else events[name][rsrc_owner].push_back(std::move(new_entry));

	return true;
}

bool ResourceSystem::remove_event(const std::string& name, Script* script)
{
	if (!script)
		return false;

	auto rsrc_owner = script->get_owner();
	if (!rsrc_owner)
		return false;

	// find all resources that register this event

	if (auto it_event = events.find(name); it_event != events.end())
	{
		auto& rsrc_events = it_event->second;

		// find the script's resource in the event resource list

		if (auto it_rsrc_event = rsrc_events.find(rsrc_owner); it_rsrc_event != rsrc_events.end())
		{
			auto& rsrc_script_events = it_rsrc_event->second;

			for (auto it = rsrc_script_events.begin(); it != rsrc_script_events.end(); ++it)
			{
				// make sure we remove the event from the correct script

				if (it->script == script)
				{
					rsrc_script_events.erase(it);

					if (rsrc_script_events.empty())
						rsrc_events.erase(rsrc_owner);

					if (rsrc_events.empty())
						events.erase(name);

					return true;
				}
			}
		}
	}

	return false;
}

ResourceResult ResourceSystem::start_resource(const std::string& name)
{
	std::lock_guard lock(mtx);

	if (const auto it = resources.find(name); it != resources.end())
	{
		const auto rsrc = it->second;

		if (rsrc->get_status() == ResourceStatus_Running)
			return ResourceResult_AlreadyStarted;

#ifdef JC_SERVER
		// read the resource meta to see if it's ok before
		// starting
		
		ResourceVerificationCtx ctx;

		if (const auto verify_result = read_meta(name, &ctx); verify_result != ResourceVerification_Ok)
			return verify_result;

		// if verification was successful then build the resource
		// and notify all clients

		rsrc->build_with_verification_ctx(ctx);
#endif

		const auto result = rsrc->start();

		if (result == ResourceResult_Ok)
			trigger_event(jc::script::event::ON_RSRC_START, rsrc);

#ifdef JC_SERVER
		// syncing a resource will start the resource so no need
		// to sync resource start action
		
		if (result == ResourceResult_Ok)
			g_net->sync_resource(name);
#endif

		return result;
	}

	return ResourceResult_NotExists;
}

ResourceResult ResourceSystem::stop_resource(const std::string& name)
{
	std::lock_guard lock(mtx);

	if (const auto it = resources.find(name); it != resources.end())
	{
		const auto rsrc = it->second;

		if (rsrc->get_status() == ResourceStatus_Stopped)
			return ResourceResult_AlreadyStopped;

		const auto result = rsrc->stop();

		if (result == ResourceResult_Ok)
			trigger_event(jc::script::event::ON_RSRC_STOP, rsrc);

#ifdef JC_SERVER
		// if the resource was stopped then we will notify all clients
		// to stop resource 

		if (result == ResourceResult_Ok)
			g_net->send_broadcast(nullptr, Packet(PlayerClientPID_ResourceAction, ChannelID_PlayerClient, name, ResourceResult_Stop));
#endif

		return result;
	}

	return ResourceResult_NotExists;
}
ResourceResult ResourceSystem::restart_resource(const std::string& name)
{
	std::lock_guard lock(mtx);

	if (const auto stop_result = stop_resource(name); stop_result != ResourceResult_Ok && stop_result != ResourceResult_AlreadyStopped)
		return stop_result;

	return start_resource(name);
}

Resource* ResourceSystem::get_resource(const std::string& name) const
{
	std::lock_guard lock(mtx);

	const auto it = resources.find(name);
	return it != resources.end() ? it->second : nullptr;
}

#ifdef JC_CLIENT
Resource* ResourceSystem::create_resource(const std::string& name)
{
	if (const auto rsrc = get_resource(name))
		return rsrc;

	const auto rsrc = JC_ALLOC(Resource, RESOURCES_FOLDER() + name + '\\', name);

	resources.insert({ name, rsrc });

	logt(YELLOW, "Client resource '{}' created", name);

	return rsrc;
}
#else
ResourceVerification ResourceSystem::verify_resource(const std::string& rsrc_name, ResourceVerificationCtx* ctx)
{
	std::lock_guard lock(mtx);

	if (rsrc_name.find_first_not_of(Resource::ALLOWED_CHARACTERS()) != std::string::npos)
		return logbtc(ResourceVerification_InvalidName, RED, "Resource '{}' has an invalid name (only '_' and alphanumeric characters allowed)", rsrc_name);

	return read_meta(rsrc_name, ctx);
}

ResourceVerification ResourceSystem::read_meta(const std::string& rsrc_name, ResourceVerificationCtx* ctx)
{
	std::lock_guard lock(mtx);

	const auto rsrc_path = ctx->path = RESOURCES_FOLDER() + rsrc_name + '\\';

	json meta;

	if (!jc_json::from_file(meta, rsrc_path + Resource::META_FILE()))
		return logbtc(ResourceVerification_InvalidMeta, RED, "Resource '{}' has an invalid meta file (or no file at all)", rsrc_name);

	if (!jc_json::get_field(meta, "author", ctx->author))				ctx->author = "unknown";
	if (!jc_json::get_field(meta, "version", ctx->version))				ctx->version = "unknown";
	if (!jc_json::get_field(meta, "description", ctx->description))		ctx->description = "none";

	// get scripts

	if (json scripts_list; jc_json::get_field(meta, "scripts", scripts_list))
	{
		for (json& script_info : scripts_list)
		{
			std::string source, type;

			if (!jc_json::get_field(script_info, "src", source))
				return logbtc(ResourceVerification_InvalidScriptSource, RED, "Resource '{}' has an invalid script source path", rsrc_name);

			if (!jc_json::get_field(script_info, "type", type))
				return logbtc(ResourceVerification_InvalidScriptType, RED, "Resource '{}' has an invalid script type", rsrc_name);

			const auto type_value = util::hash::JENKINS(type);

			// save scripts only if they match their running machine type

			const auto fullpath = rsrc_path + source;

			if (!std::filesystem::is_regular_file(fullpath))
				return logbtc(ResourceVerification_ScriptNotExists, RED, "Resource '{}', script '{}' does not exist", rsrc_name, source);

			switch (type_value)
			{
			case ScriptType_Client: ctx->client.scripts.insert({ source, { source, util::fs::get_last_write_time(fullpath), ScriptType_Client, type_value } }); break;
			case ScriptType_Shared: ctx->shared.scripts.insert({ source, { source, util::fs::get_last_write_time(fullpath), ScriptType_Shared, type_value } }); break;
			case ScriptType_Server: ctx->server.scripts.insert({ source, { source, util::fs::get_last_write_time(fullpath), ScriptType_Server, type_value } }); break;
			default: return logbtc(ResourceVerification_InvalidScriptList, RED, "Resource '{}', script '{}' has an invalid script file type", rsrc_name, source);
			}
		}
	}
	else return logbtc(ResourceVerification_InvalidScriptList, RED, "Resource '{}' has an invalid script list", rsrc_name);

	// get files

	if (json files_list; jc_json::get_field(meta, "files", files_list))
	{
		for (const std::string& filename : files_list)
		{
			const auto fullpath = rsrc_path + filename;

			if (!std::filesystem::is_regular_file(fullpath))
				return logbtc(ResourceVerification_ScriptNotExists, RED, "Resource '{}', file '{}' does not exist", rsrc_name, filename);

			ctx->client.files.insert({ filename, { filename, util::fs::get_last_write_time(fullpath), ScriptType_NoScript } });
		}
	}

	return ResourceVerification_Ok;
}
#endif