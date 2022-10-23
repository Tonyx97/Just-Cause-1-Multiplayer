#include <defs/standard.h>

#include "npc_variant.h"

shared_ptr<NPCVariant> NPCVariant::CREATE()
{
	shared_ptr<NPCVariant> rf;

	jc::std_call(jc::npc_variant::fn::CREATE, &rf);

	return rf;
}