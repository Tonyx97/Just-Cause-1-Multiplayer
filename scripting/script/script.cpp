#include <defs/standard.h>

#include "script.h"
#include "lua_ctx/script_registering.h"

#include <resource/resource.h>

Script::Script(
	Resource* rsrc,
	const std::string& path,
	const std::string& name,
	ScriptType type) :
	owner(rsrc),
	path(path),
	name(name),
	type(type)
{
}

Script::~Script()
{
	stop();
}

void Script::start()
{
	vm = JC_ALLOC(luas::ctx, true);

	if (const auto data = util::fs::read_plain_file(path); !data.empty())
	{
		// register functions and globals first

		script::register_functions(this);
		script::register_globals(this);

		// execute the script
		
		vm->exec_string(data.data());
	}
}

void Script::stop()
{
	JC_FREE(vm);
	
	vm = nullptr;
}

const std::string& Script::get_rsrc_path() const
{
	return owner->get_path();
}

const std::string& Script::get_rsrc_name() const
{
	return owner->get_name();
}