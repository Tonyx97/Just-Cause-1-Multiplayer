#pragma once

namespace jc::bug_ripper
{
	inline bool exception_catch_enabled = true;

	bool init(void* mod_base);
	bool destroy();
	bool reroute_exception_handler(bool place);

	long show_and_dump_crash(struct _EXCEPTION_POINTERS* ep);
}