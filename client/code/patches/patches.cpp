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
	const auto stance_controller = character->get_stance_controller();
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

	// avoid weird 2d sounds

	jc::hooks::hook<jc::proto::patches::ambience_2d_sounds>(&hk_play_ambience_2d_sounds);	// I think this causes crashes sometimes, we have to find a better way
	jc::hooks::hook<jc::proto::patches::character_can_be_destroyed>(&hk_character__can_be_destroyed);
}

void jc::patches::undo()
{
	head_rotation_patch._undo();

	jc::hooks::unhook<jc::proto::patches::character_can_be_destroyed>();
	jc::hooks::unhook<jc::proto::patches::ambience_2d_sounds>();
}