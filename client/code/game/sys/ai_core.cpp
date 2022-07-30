#include <defs/standard.h>

#include "ai_core.h"

#include <game/object/character/character.h>

void AiCore::init()
{
	Character::SET_FLYING_Y_MODIFIER(5.f);
	Character::SET_GLOBAL_PUNCH_DAMAGE(500.f);
	Character::SET_GLOBAL_PUNCH_DAMAGE(500.f, true);
}

void AiCore::destroy()
{
	Character::SET_FLYING_Y_MODIFIER(1.f);
	Character::SET_GLOBAL_PUNCH_DAMAGE(40.f, true);
	Character::SET_GLOBAL_PUNCH_DAMAGE(40.f);
}

void AiCore::insert_character_handle(CharacterHandle* handle)
{
	CharacterHandleEntry result;

	jc::this_call<ptr>(jc::ai_core::fn::INSERT_CHARACTER_HANDLE, this, &result, handle);
}

CharacterHandle* AiCore::get_character_handle_from_entry(const CharacterHandleEntry* v) const
{
	return jc::this_call<CharacterHandle*>(jc::ai_core::fn::GET_CHARACTER_HANDLE_FROM_ID, v);
}