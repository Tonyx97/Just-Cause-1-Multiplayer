#include <defs/standard.h>

#include "patches.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/sys/all.h>

#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>

#include <mp/net.h>

DEFINE_HOOK_THISCALL(play_ambience_2d_sounds, 0x656ED0, jc::stl::string*, int a1, jc::stl::string* a2)
{
	const auto res = play_ambience_2d_sounds_hook.call(a1, a2);

	if (a2)
		*a2 = "";
		
	return res;
}

namespace jc::patches
{
#if FAST_LOAD
	// savegame integrity check patch so we can change the buffer to proper stuff
	// 
	patch<2> savegame_integrity_patch(0x4925D5);

	// minimal savegame buffer deserialization patch
	// 
	patch<5> savegame_minimal_load_patch(0x4F3D02);

	// avoid CInfoMessage to show stuff that we don't want to,
	// this happens because we don't setup the localizations
	// so messages like "You were killed" would show up etc
	//
	patch<5> info_message_patch(0x7E2632);
#endif

	// sets the head interpolation value to 1.0 all the time
	// so we can set our own rotation values and avoid the engine
	// setting its own values (we load 1.0 and jump straight to the
	// head interpolation writing instruction)
	//
	patch<13> head_rotation_patch(0x64B1F4);

	// avoids the engine to drop the current weapon upon a character's death
	// because this causes issues with the weapon sync
	//
	patch<2> drop_weapon_on_death_patch(0x590810);

	// avoids the idle animation when the local player is standing still
	// and not moving
	//
	patch<2> no_idle_animation(0x591443);

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

void __fastcall hk_head_rotation_patch(Skeleton* skeleton, ptr _)
{
	// process the head interpolation and movement logic if
	// it's local player or it's aiming, we need to set our own head rotation
	// for remote players who aren't aiming, only looking

	if (const auto player = g_net->get_player_by_character(skeleton->get_character()))
		if (player->is_local() || player->is_hip_aiming() || player->is_full_aiming())
		{
			const auto delta = g_time->get_delta();

			const float head_interpolation = skeleton->get_head_interpolation();

			if (skeleton->get_head_interpolation_factor() == 0.f)
				skeleton->set_head_interpolation(head_interpolation - 1.f * delta);
			else skeleton->set_head_interpolation(1.f * delta + head_interpolation);
		}
}

void jc::patches::apply_initial_patches()
{
#if FAST_LOAD
	// skips the loading screen

	jc::nop(0x46C859, 5);

	// forces update on GuiLoadSave objects

	jc::write(0x74ui8, 0x7FED33);
#endif

	// change the name of the window
	
	std::strcpy((char*)0xAEBE18, "JC:MP\0");

	// shadow fix (thanks to ThirteenAG)

	jc::write(0xEBui8, 0x46DEA5);

#ifdef JC_DBG
	g_game_ctx->set_window_resolution(1600, 1200);
	g_game_ctx->set_fullscreen(false);
#else
	int32_t sx, sy;

	util::win::get_desktop_resolution(sx, sy);

	g_game_ctx->set_window_resolution(sx, sy);
	g_game_ctx->set_fullscreen(true);
#endif
}

void jc::patches::apply()
{
	// replace 10 with 1 so the alt-tab is faster and doesn't freeze

	jc::write(1, jc::g::FOCUS_FREEZE_TIME);

	// remove the randomness in the character's punch damage

	jc::write(1ui8, 0x5A4400);

	// make npcs look at other npcs when they are less than 0 meters (aka never lol)

	jc::write_protect(0.f, jc::g::patch::CHAR_LOOK_MIN_DISTANCE.address);

	// change this protection to RWX so we can bypass the recreation of the weapon belt when initializing/setting
	// a character's models

	jc::protect_v(PAGE_EXECUTE_READWRITE, jc::g::patch::AVOID_WEAPON_BELT_RECREATION_WHILE_CHAR_INIT);

#if FAST_LOAD
	// apply CInfoMessage patch

	info_message_patch._do(
	{
		0xE9, 0xFA, 0x00, 0x00, 0x00
	});

	// patch savegame integrity check

	savegame_integrity_patch._do(
	{
		0xEB, 0x23,
	});

	// patch savegame deserialization

	savegame_minimal_load_patch._do(
	{
		0xE9, 0xA0, 0x02, 0x00, 0x00
	});
#endif

	// apply head rotation patch

	auto head_rotation_patch_offset = jc::calc_call_offset(0x64B1F4 + 6, hk_head_rotation_patch);

	head_rotation_patch._do(
	{
		0x8B, 0x8D, 0xBC, 0xFD, 0xFF, 0xFF, // mov ecx, [ebp-244h]
		0xE8,								// call hook
		head_rotation_patch_offset.b0,
		head_rotation_patch_offset.b1,
		head_rotation_patch_offset.b2,
		head_rotation_patch_offset.b3,
		0xEB, 0x50							// jmp
	});

	// apply drop weapon on death patch

	drop_weapon_on_death_patch._do(
	{
		0xEB, 0x2E
	});

	// apply no idle stance patch

	no_idle_animation._do(
	{
		0xEB, 0x19
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

	// apply character's fly stance to local's patch
	// this avoids the engine setting the flying npc animation and will
	// use the player one instead

	jc::nop(0x5A3D83, 18);

	// avoid weird 2d sounds

	play_ambience_2d_sounds_hook.hook();
}

void jc::patches::undo()
{
	death_camera_velocity._undo();
	set_health_red_fx._undo();
	death_state._undo();
	no_idle_animation._undo();
	drop_weapon_on_death_patch._undo();
	head_rotation_patch._undo();

#if FAST_LOAD
	savegame_minimal_load_patch._undo();
	savegame_integrity_patch._undo();
	info_message_patch._undo();
#endif

	play_ambience_2d_sounds_hook.unhook();

	jc::write(20ui8, 0x5A4400);
}