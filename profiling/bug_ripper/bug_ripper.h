#pragma once

namespace jc::bug_ripper
{
	bool init(void* mod_base);
	bool destroy();

	long show_and_dump_crash(struct _EXCEPTION_POINTERS* ep);
}