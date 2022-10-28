#include <defs/standard.h>

#include "pfx_character.h"

void PfxCharacter::set_capsule_dimensions(float height, float unk, float radius)
{
	jc::this_call(jc::pfx_character::fn::SET_CAPSULE_DIMENSIONS, instance.get(), height, unk, radius);
}