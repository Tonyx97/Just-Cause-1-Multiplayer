#include <defs/standard.h>

#include "script_registering.h"
#include "script_globals.h"

#include <resource_sys/resource_system.h>

void jc::script::register_functions(Script* script)
{
	const auto vm = script->get_vm();

#if defined(JC_CLIENT)
	// register client functions
#elif defined(JC_SERVER)
	// register server functions
#endif

	// resgister shared functions

	vm->add_function("cancelEvent", []() { g_rsrc->cancel_event(); });

	vm->add_function("addEvent", [](luas::state& s, const std::string& event_name, luas::lua_fn& fn, luas::variadic_args va)
	{
		const auto script = s.get_global_var<Script*>(jc::script::globals::SCRIPT_INSTANCE);
		const bool allow_remote_trigger = va.size() == 1 ? va.get<bool>(0) : false;

		return g_rsrc->add_event(event_name, fn, script, allow_remote_trigger);
	});

	vm->add_function("removeEvent", [](luas::state& s, const std::string& event_name)
	{
		const auto script = s.get_global_var<Script*>(jc::script::globals::SCRIPT_INSTANCE);

		return g_rsrc->remove_event(event_name, script);
	});

	vm->add_function("triggerEvent", [](const std::string& name, luas::variadic_args va)
	{
		return g_rsrc->trigger_non_remote_event(name, va);
	});
}

void jc::script::register_globals(Script* script)
{
	const auto vm = script->get_vm();

	vm->add_global(jc::script::globals::SCRIPT_INSTANCE, script);
	vm->add_global(jc::script::globals::RSRC_PATH, script->get_rsrc_path());
	vm->add_global(jc::script::globals::RSRC_NAME, script->get_rsrc_name());
}