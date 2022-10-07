#include <defs/standard.h>

#include "resource.h"

Resource::Resource(const std::string& name, const ResourceVerificationCtx& ctx) : name(name)
{
	path = ctx.path;
	author = ctx.author;
	version = ctx.version;
	description = ctx.description;

	// create scripts (in suspended state ofc)

	auto create_add_script = [&](const ScriptCtx& script_ctx, const auto& script_name)
	{
		const auto script_path = ctx.path + script_name;
		const auto script = JC_ALLOC(Script, script_path, script_name, script_ctx.type);

		scripts.insert({ script_name, script });
	};

	// save client and shared files when client or server is running
	// (server must know all client, server and shared files)

	client_files = ctx.client;
	shared_files = ctx.shared;

	// client will load scripts inside the client and shared list
	// and server will load scripts inside server and shared list

#if defined(JC_CLIENT)
	for (const auto& [script_name, script_ctx] : ctx.client.scripts)
#else
	// save server files if we are running the server

	server_files = ctx.server;

	for (const auto& [script_name, script_ctx] : ctx.server.scripts)
#endif
		create_add_script(script_ctx, script_name);

	// load shared scripts in both ends

	for (const auto& [script_name, script_ctx] : ctx.shared.scripts)
		create_add_script(script_ctx, script_name);

#if defined(JC_SERVER)
	// get the total size of client files

	client_files_total_size = calculate_total_client_file_size();
#endif
}

Resource::~Resource()
{
	for (const auto& [_, script] : scripts)
		JC_FREE(script);
}

#ifdef JC_CLIENT
#else
size_t Resource::calculate_total_client_file_size()
{
	size_t size = 0u;

	for_each_client_file([&](const std::string& filename, const FileCtx* ctx)
	{
		size += static_cast<size_t>(util::fs::file_size(path + filename));
	});

	return size;
}

size_t Resource::get_total_client_file_size()
{
	return client_files_total_size;
}
#endif

ResourceResult Resource::start()
{
	logt(YELLOW, "Starting '{}'... ", name);

	for (const auto& [script_name, script] : scripts)
		script->start();

	status = ResourceStatus_Running;

	return ResourceResult_Ok;
}

ResourceResult Resource::stop()
{
	logt(YELLOW, "Stopping '{}'... ", name);

	for (const auto& [script_name, script] : scripts)
		script->stop();

	status = ResourceStatus_Stopped;

	return ResourceResult_Ok;
}

ResourceResult Resource::restart()
{
	logt(YELLOW, "Restarting '{}'... ", name);

	stop();
	start();

	return ResourceResult_Ok;
}