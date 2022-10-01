#include <defs/standard.h>
#include <defs/json.h>

#include "resource_system.h"

#include <mp/net.h>

#include <resource/resource.h>

namespace resource_system
{
	void create_resource_system()
	{
		check(!g_rsrc, "Resource system must not exist before creating it");

		g_rsrc = std::make_unique<ResourceSystem>();
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
	for (const auto& [name, rsrc] : resources)
		JC_FREE(rsrc);

	resources.clear();
}

void ResourceSystem::refresh()
{
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
		if (!resources.contains(rsrc_name))
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

ResourceVerification ResourceSystem::verify_resource(const std::string& rsrc_name, ResourceVerificationCtx* ctx)
{
	// check if this resource name was already registered

	if (resources.contains(rsrc_name))
		return logbtc(ResourceVerification_AlreadyExists, RED, "Resource '{}' already exists", rsrc_name);

	if (rsrc_name.find_first_not_of(Resource::ALLOWED_CHARACTERS()) != std::string::npos)
		return logbtc(ResourceVerification_InvalidName, RED, "Resource '{}' has an invalid name (only '_' and alphanumeric characters allowed)", rsrc_name);

	const auto rsrc_path = ctx->path = RESOURCES_FOLDER() + rsrc_name + '\\';

	json meta;

	if (!jc_json::from_file(meta, rsrc_path + Resource::META_FILE()))
		return logbtc(ResourceVerification_InvalidMeta, RED, "Resource '{}' has an invalid meta file (or no file at all)", rsrc_name);

	if (!jc_json::get_field(meta, "author", ctx->author))				ctx->author = "unknown";
	if (!jc_json::get_field(meta, "version", ctx->version))				ctx->version = "unknown";
	if (!jc_json::get_field(meta, "description", ctx->description))		ctx->description = "none";

	if (json scripts_list; jc_json::get_field(meta, "scripts", scripts_list))
	{
		for (json& script_info : scripts_list)
		{
			std::string source, type;

			if (!jc_json::get_field(script_info, "src", source))
				return logbtc(ResourceVerification_InvalidScriptSource, RED, "Resource '{}' has an invalid script source path", rsrc_name);

			if (!jc_json::get_field(script_info, "type", type))
				return logbtc(ResourceVerification_InvalidScriptType, RED, "Resource '{}' has an invalid script type", rsrc_name);

			if (!std::filesystem::is_regular_file(rsrc_path + source))
				return logbtc(ResourceVerification_ScriptNotExists, RED, "Resource '{}', script '{}' does not exist", rsrc_name, source);

			ctx->scripts.insert({ source, { source, type } });
		}
	}
	else return logbtc(ResourceVerification_InvalidScriptList, RED, "Resource '{}' has an invalid script list", rsrc_name);

	return ResourceVerification_Ok;
}

ResourceResult ResourceSystem::start_resource(const std::string& name)
{
	if (const auto it = resources.find(name); it != resources.end())
		return start_resource(it->second);

	return ResourceResult_NotExists;
}

ResourceResult ResourceSystem::start_resource(Resource* rsrc)
{

	return rsrc->start();
}