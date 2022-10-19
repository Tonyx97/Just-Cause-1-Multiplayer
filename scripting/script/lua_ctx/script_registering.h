#pragma once

namespace luas { class ctx; }

class Script;

namespace script
{
	void register_functions(Script* script);
	void register_globals(Script* script);
}