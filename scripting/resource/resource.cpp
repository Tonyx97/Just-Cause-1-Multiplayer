#include <defs/standard.h>

#include "resource.h"

Resource::Resource(const std::string& name, const ResourceVerificationCtx& ctx) : name(name)
{
	path = ctx.path;
	author = ctx.author;
	version = ctx.version;
	description = ctx.description;

	// create scripts (in suspended state ofc)

	for (const auto& [script_name, script_ctx] : ctx.scripts)
	{
		const auto script_path = ctx.path + script_name;
		const auto script = JC_ALLOC(Script, script_path, script_name, script_ctx.type);

		scripts.insert({ script_name, script });
	}
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

	return ResourceResult_Ok;
}

ResourceResult Resource::stop()
{
	logt_nl(YELLOW, "Stopping '{}'... ", name);

	log(GREEN, "OK");

	return ResourceResult_Ok;
}

ResourceResult Resource::restart()
{
	logt_nl(YELLOW, "Restarting '{}'... ", name);

	log(GREEN, "OK");

	return ResourceResult_Ok;
}