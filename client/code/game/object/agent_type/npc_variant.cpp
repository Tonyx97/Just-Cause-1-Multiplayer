#include <defs/standard.h>

#include "npc_variant.h"

ref<NPCVariant> NPCVariant::CREATE()
{
	ref<NPCVariant> rf;

	jc::std_call(jc::npc_variant::fn::CREATE, &rf);

	return rf;
}