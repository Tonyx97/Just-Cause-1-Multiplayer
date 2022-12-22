#pragma once

#include <luas.h>

#include "lua_ctx/script_events.h"

DEFINE_ENUM(ScriptType, uint32_t)
{
	ScriptType_Invalid	= 0u,
	ScriptType_NoScript	= 1u,
	ScriptType_Client	= util::hash::JENKINS("client"),
	ScriptType_Server	= util::hash::JENKINS("server"),
	ScriptType_Shared	= util::hash::JENKINS("shared"),
};

class Resource;
class Script;

struct ScriptEventInfo
{
	Script* script;

	luas::lua_fn fn;

	bool allow_remote_trigger;
};

struct ScriptCmdInfo
{
	Script* script;

	luas::lua_fn fn;
};

class Script
{
private:

	std::string path;
	std::string name;

	Resource* owner = nullptr;

	luas::ctx* vm = nullptr;

	ScriptType type = ScriptType_Invalid;

public:

	Script(
		Resource* rsrc,
		const std::string& path,
		const std::string& name,
		ScriptType type);

	~Script();

	void start();
	void stop();

	Resource* get_owner() const { return owner; }

	luas::ctx* get_vm() const { return vm; }

	const std::string& get_rsrc_path() const;
	const std::string& get_rsrc_name() const;
	const std::string& get_path() const { return path; }
	const std::string& get_name() const { return name; }
};