#pragma once

#include "pfx_instance.h"

namespace jc::pfx_character
{
	static constexpr uint32_t MASS	= 0x80;

	namespace fn
	{
		static constexpr uint32_t SET_CAPSULE_DIMENSIONS = 0x745C40;
	}
}

class PfxCharacter : public PfxInstance
{
private:
public:

	void set_capsule_dimensions(float height, float unk, float radius);
};