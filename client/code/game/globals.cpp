#include <defs/standard.h>

#include "globals.h"

namespace jc::game
{
	void* malloc_internal(int size)
	{
		return jc::c_call<void*, int>(jc::g::memory::fn::MALLOC, size);
	}
}