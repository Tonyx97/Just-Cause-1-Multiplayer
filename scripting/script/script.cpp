#include <defs/standard.h>

#include "script.h"

#include <luas.h>

Script::Script(const std::string& path, const std::string& name, ScriptType type) : path(path), name(name), type(type)
{
}

Script::~Script()
{
	stop();
}

void Script::start()
{
	vm = JC_ALLOC(luas::ctx);

	if (const auto data = util::fs::read_plain_file(path); !data.empty())
		vm->exec_string(data.data());
}

void Script::stop()
{
	JC_FREE(vm);
	
	vm = nullptr;
}