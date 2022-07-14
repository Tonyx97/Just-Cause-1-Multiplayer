#include <defs/standard.h>

#include "patches.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/sys/all.h>

#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>

jc::stl::string* __fastcall hk_play_ambience_2d_sounds(int a1, void*, jc::stl::string* a2)
{
	auto res = jc::hooks::call<jc::proto::patches::ambience_2d_sounds>(a1, a2);

	if (a2)
		*a2 = "";

	return res;
}

bool __fastcall hk_character__can_be_destroyed(Character* character)
{
	// if our local player is being checked here, it means PlayerSettings is trying to
	// get us to the game continue menu, we will return false all the time so we can implement our
	// own spawing and also we avoid the stupid menu that serves no purpose

	if (character == g_world->get_localplayer_character())
		return false;

	// we implement our own logic of this function, we can modify the amount of time
	// a character can stay death before its destruction or we can just skip it in the future
	// and so on, this will vary in future development

	const auto stance_controller = character->get_body_stance();
	const auto movement_id		 = stance_controller->get_movement_id();

	return character->get_max_hp() <= std::numeric_limits<float>::max() &&
		   (movement_id == 0x34 || movement_id == 0x62) &&
		   character->get_death_time() > 10.f;
}

namespace jc::patches
{
	// sets the head interpolation value to 1.0 all the time
	// so we can set our own rotation values and avoid the engine
	// setting its own values (we load 1.0 and jump straight to the
	// head interpolation writing instruction)
	//
	patch<8> head_rotation_patch(0x64B1F4);

	// avoids the trashy blur that happens when the localplayer dies,
	// but most importantly, the state is not reset such as the UI etc
	// this allows us to directly respawn our localplayer's character
	// so it works without too much overkill
	//
	patch<5> death_state(0x4D15BF);

	// avoids the red effect when the localplayer is low on health
	//
	patch<5> set_health_red_fx(0x5981F9);

	// do not make the death camera slower, just keep rotating baby
	//
	patch<2> death_camera_velocity(0x605755);
}

void jc::patches::apply()
{
	// replace 10 with 1 so the alt-tab is faster and doesn't freeze

	jc::write(1, jc::g::FOCUS_FREEZE_TIME);

	// make npcs look at other npcs when they are less than 0 meters (aka never lol)

	jc::write_protect(0.f, jc::g::patch::CHAR_LOOK_MIN_DISTANCE.address);

	// change this protection to RWX so we can bypass the recreation of the weapon belt when initializing/setting
	// a character's models

	jc::protect_v(PAGE_EXECUTE_READWRITE, jc::g::patch::AVOID_WEAPON_BELT_RECREATION_WHILE_CHAR_INIT);

	// apply head rotation patch

	head_rotation_patch._do(
	{
		0xD9, 0x05, 0xB4, 0xD2, 0xAE, 0x00,	// fld dword ptr [0xAED2B4]
		0xEB, 0x49							// jmp
	});

	// apply death state hiding patch

	death_state._do(
	{
		0xE9, 0x61, 0x06, 0x00, 0x00		// jmp
	});

	// apply set health red effect patch

	set_health_red_fx._do(
	{
		0xE9, 0x44, 0x01, 0x00, 0x00		// jmp
	});

	// apply death camera rotation patch

	death_camera_velocity._do(
	{
		0xEB, 0x66	// jmp
	});

	// avoid weird 2d sounds

	jc::hooks::hook<jc::proto::patches::ambience_2d_sounds>(&hk_play_ambience_2d_sounds);

	// implement our own logic for Character::CanBeDestroyed

	jc::hooks::hook<jc::proto::patches::character_can_be_destroyed>(&hk_character__can_be_destroyed);
}

void jc::patches::undo()
{
	death_camera_velocity._undo();
	set_health_red_fx._undo();
	death_state._undo();
	head_rotation_patch._undo();

	jc::hooks::unhook<jc::proto::patches::character_can_be_destroyed>();
	jc::hooks::unhook<jc::proto::patches::ambience_2d_sounds>();
}