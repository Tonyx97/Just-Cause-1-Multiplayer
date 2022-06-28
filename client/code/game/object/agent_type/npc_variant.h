#pragma once

#include "../base/base.h"

namespace jc::npc_variant
{
	namespace fn
	{
		static constexpr uint32_t CREATE	= 0x66E5C0;
		static constexpr uint32_t ALLOCATE	= 0x671D30;
		static constexpr uint32_t SETUP		= 0x816390;
	}
}

class NPCVariant : public ObjectBase
{
public:

	static ref<NPCVariant> CREATE();
};