#include <defs/standard.h>

#include "script.h"

#include <luas.h>

Script::Script(const std::string& path, const std::string& name, ScriptType type) : path(path), name(name), type(type)
{
	log(PURPLE, "script {} {} loaded ({:x})", path, name, type);
	start();
}

Script::~Script()
{
	stop();
}

void Script::start()
{
	vm = JC_ALLOC(luas::ctx);
}

void Script::stop()
{
	JC_FREE(vm);
}

void Script::restart()
{
	stop();
	start();
}