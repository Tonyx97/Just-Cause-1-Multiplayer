#include <defs/standard.h>

#include "resource.h"

Resource::Resource(const std::string& name, const ResourceVerificationCtx& ctx) : name(name)
{
	path = ctx.path;
	author = ctx.author;
	version = ctx.version;
	description = ctx.description;

	// create scripts (in suspended state ofc)

	auto create_add_script = [&](const ResourceVerificationCtx::ScriptCtx& script_ctx, const auto& script_name)
	{
		const auto script_path = ctx.path + script_name;
		const auto script = JC_ALLOC(Script, script_path, script_name, script_ctx.type);

		scripts.insert({ script_name, script });
	};

	// client will load scripts inside the client and shared list
	// and server will load scripts inside server and shared list

#if defined(JC_CLIENT)
	for (const auto& [script_name, script_ctx] : ctx.client.scripts)
#else
	for (const auto& [script_name, script_ctx] : ctx.server.scripts)
#endif
		create_add_script(script_ctx, script_name);

	// load shared scripts in both ends

	for (const auto& [script_name, script_ctx] : ctx.shared.scripts)
		create_add_script(script_ctx, script_name);
}

Resource::~Resource()
{
	for (const auto& [_, script] : scripts)
		JC_FREE(script);
}

ResourceResult Resource::start()
{
	logt_nl(YELLOW, "Starting '{}'... ", name);

	log(GREEN, "OK");

	for (const auto& [script_name, script] : scripts)
		script->start();

	status = ResourceStatus_Running;

	return ResourceResult_Ok;
}

ResourceResult Resource::stop()
{
	logt_nl(YELLOW, "Stopping '{}'... ", name);

	for (const auto& [script_name, script] : scripts)
		script->stop();

	status = ResourceStatus_Stopped;

	log(GREEN, "OK");

	return ResourceResult_Ok;
}

ResourceResult Resource::restart()
{
	logt_nl(YELLOW, "Restarting '{}'... ", name);

	stop();
	start();

	log(GREEN, "OK");

	return ResourceResult_Ok;
}