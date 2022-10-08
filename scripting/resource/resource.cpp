#include <defs/standard.h>

#include "resource.h"

void Resource::clear()
{
#ifdef JC_SERVER
	destroy_file_lists();
#endif

	destroy_scripts();
}

#ifdef JC_CLIENT
Resource::Resource(const std::string& name) : name(name)
{
}
#else
Resource::Resource(const std::string& name, const ResourceVerificationCtx& ctx) : name(name)
{
	build_with_verification_ctx(ctx);
}

void Resource::destroy_file_lists()
{
	client_files.clear();
	shared_files.clear();
	server_files.clear();
}

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

void Resource::build_with_verification_ctx(const ResourceVerificationCtx& ctx)
{
	check(status == ResourceStatus_Stopped, "Resource must be stopped before building");

	clear();

	path = ctx.path;
	author = ctx.author;
	version = ctx.version;
	description = ctx.description;

	client_files = ctx.client;
	shared_files = ctx.shared;
	server_files = ctx.server;

	for (const auto& [script_name, script_ctx] : ctx.client.scripts) create_script(script_ctx, ctx.path, script_name);
	for (const auto& [script_name, script_ctx] : ctx.shared.scripts) create_script(script_ctx, ctx.path, script_name);

	// get the total size of client files

	client_files_total_size = calculate_total_client_file_size();
}
#endif

Resource::~Resource()
{
	clear();
}

void Resource::destroy_scripts()
{
	check(is_stopped(), "Resource must be stopped to destroy scripts");

	for (const auto& [_, script] : scripts)
		JC_FREE(script);

	scripts.clear();
}

void Resource::create_script(const ScriptCtx& script_ctx, const std::string& path, const std::string& script_name)
{
	const auto script_path = path + script_name;
	const auto script = JC_ALLOC(Script, script_path, script_name, script_ctx.type);

	scripts.insert({ script_name, script });
}

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
	if (status == ResourceStatus_Stopped)
		return ResourceResult_AlreadyStopped;

	logt(YELLOW, "Stopping '{}'... ", name);

	for (const auto& [script_name, script] : scripts)
		script->stop();

	status = ResourceStatus_Stopped;

	return ResourceResult_Ok;
}