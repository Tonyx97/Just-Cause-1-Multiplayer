#include <defs/standard.h>

#include "ai_core.h"

void AiCore::init()
{
}

void AiCore::destroy()
{
}

void AiCore::insert_character_handle(CharacterHandle* handle)
{
	CharacterHandleEntry result;

	jc::this_call<ptr>(jc::ai_core::fn::INSERT_CHARACTER_HANDLE, this, &result, handle);
}