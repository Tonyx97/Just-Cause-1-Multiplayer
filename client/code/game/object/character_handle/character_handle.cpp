#include <defs/standard.h>

#include "../character/character.h"

#include "character_handle.h"

#include <game/sys/ai/ai_core.h>

void CharacterHandle::set_pending_destroy()
{
	jc::write<bool>(true, this, jc::character_handle::PENDING_TO_DESTROY);
}

CharacterHandle* CharacterHandle::GET_FREE_HANDLE()
{
	return jc::c_call<CharacterHandle*>(jc::character_handle::fn::GET_FREE_HANDLE, 0x1A8, 0xA);
}

void CharacterHandle::destroy()
{
	get_character()->remove_flag(1 << 6);

	set_pending_destroy();
}

void CharacterHandle::respawn()
{
	get_character()->respawn();
}

bool CharacterHandle::is_looking_at_any_npc() const
{
	return jc::read<bool>(this, jc::character_handle::LOOKING_AT_ANY_NPC);
}

CharacterHandle* CharacterHandle::create(CharacterInfo* info, Transform* transform, int weapon_id)
{
	jc::stl::string xml_file = "";

	jc::this_call<CharacterHandle*>(jc::character_handle::fn::CREATE_CHARACTER, this, info, transform, weapon_id, &xml_file, nullptr, nullptr);

	const auto character = get_character();

	check(character, "Character could not be created");

	// disable roll clamp by default

	character->set_roll_clamp_enabled(false);

	g_ai->insert_character_handle(this);

	return this;
}

CharacterController* CharacterHandle::get_controller() const
{
    return REF(CharacterController*, this, jc::character_handle::CHARACTER_CONTROLLER);
}

Character* CharacterHandle::get_character() const
{
	const auto character = get_character_unsafe();

	check(character, "Character handle must have a valid character");

	return character;
}

Character* CharacterHandle::get_character_unsafe() const
{
	return jc::read<Character*>(this, jc::character_handle::CHARACTER);
}