#include <defs/standard.h>

#include "ai_core.h"

#include <game/object/character/character.h>

void AiCore::init()
{
	Character::SET_GLOBAL_PUNCH_DAMAGE(500.f);
	Character::SET_GLOBAL_PUNCH_DAMAGE(500.f, true);
}

void AiCore::destroy()
{
	Character::SET_GLOBAL_PUNCH_DAMAGE(40.f, true);
	Character::SET_GLOBAL_PUNCH_DAMAGE(40.f);
}

void AiCore::insert_character_handle(CharacterHandle* handle)
{
	CharacterHandleEntry result;

	jc::this_call<ptr>(jc::ai_core::fn::INSERT_CHARACTER_HANDLE, this, &result, handle);
}