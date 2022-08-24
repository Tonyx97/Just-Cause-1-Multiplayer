#include <defs/standard.h>
#include <defs/client_basic.h>

#include "patches.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/sys/all.h>

#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/weapon/weapon.h>

#include <mp/net.h>

namespace jc::patches
{
#if FAST_LOAD
	// savegame integrity check patch so we can change the buffer to proper stuff
	// 
	patch savegame_integrity_patch(0x4925D5);

	// minimal savegame buffer deserialization patch
	// 
	patch savegame_minimal_load_patch(0x4F3D02);

	// avoid CInfoMessage to show stuff that we don't want to,
	// this happens because we don't setup the localizations
	// so messages like "You were killed" would show up etc
	//
	patch info_message_patch(0x7E2632);
#endif

	// sets the head interpolation value to 1.0 all the time
	// so we can set our own rotation values and avoid the engine
	// setting its own values (we load 1.0 and jump straight to the
	// head interpolation writing instruction)
	//
	patch head_rotation_patch(0x64B1F4);

	// avoids the engine to drop the current weapon upon a character's death
	// because this causes issues with the weapon sync
	//
	patch drop_weapon_on_death_patch(0x590810);

	// avoids the idle animation when the local player is standing still
	// and not moving
	//
	patch no_idle_animation(0x591443);

	// avoids the trashy blur that happens when the localplayer dies,
	// but most importantly, the state is not reset such as the UI etc
	// this allows us to directly respawn our localplayer's character
	// so it works without too much overkill
	//
	patch death_state(0x4D15BF);

	// avoids the red effect when the localplayer is low on health
	//
	patch set_health_red_fx(0x5981F9);

	// do not make the death camera slower, just keep rotating baby
	//
	patch death_camera_velocity(0x605755);

	// patch to properly create synced shots in remote players,
	// since the spread is clientside, the bullets could hit or not in remote players causing
	// a sense of desync, this patch will sync the rand seed across all players and each
	// player will generate the correct set of PRGN so they stay the same
	//
	patch fire_bullet_patch(0x61F6AB);

	// removes the heat haze effect from scoped weapons because it's shit
	//
	patch scope_heat_haze_patch(0x5B86AD);

	// removes engine overriding of last muzzle transform in weapon because
	// we will use our own
	//
	patch last_muzzle_transform_patch(0x61F1D9);

	// removes automatic movement from active handles to dead handles of characters
	// destruction of any character (player or npc) will be handled by the mod/scripting
	// to avoid problems with the engine itself
	//
	patch ai_core_dead_handles_patch(0x584C64);

	// patches the automatic turning of the land vehicle's engine
	//
	patch land_vehicle_engine_patch(0x85053A);
}

DEFINE_HOOK_THISCALL(play_ambience_2d_sounds, 0x656ED0, jc::stl::string*, int a1, jc::stl::string* a2)
{
	const auto res = play_ambience_2d_sounds_hook.call(a1, a2);

	if (a2)
		*a2 = "";

	return res;
}

DEFINE_HOOK_CCALL(fn_ret_1, 0x62A510, int)
{
	switch (RET_ADDRESS)
	{
	// PlayerController & NPCController handlers

	case 0x850165:	// LandVehicle check to see if the driver is an npc or a player
	// All AirVehicle checks to see if the driver is an npc or a player
	case 0x8B53DB:
	case 0x8B5B7D:
	// All Helicopter checks to see if the driver is an npc or a player
	case 0x82D398:
	case 0x82D70D:
	case 0x82D782:
	case 0x82D7F1:
		return false;
	}

	return fn_ret_1_hook.call();
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

void __fastcall hk_fire_bullet_patch(Weapon* weapon, ptr _, Transform* final_muzzle_transform)
{
	if (const auto owner = weapon->get_owner())
		if (const auto player = g_net->get_player_by_character(owner))
			if (const auto weapon_info = weapon->get_info())
			{
				//if (weapon_info->get_bullet_type() != 4)
				{
					// if the player is local then just get our aim target and calculate the
					// proper direction, otherwise, we want to grab the muzzle and aim target
					// from the remote Player instance

					const auto muzzle = player->get_muzzle_position();

					auto direction = player->get_bullet_direction();

					// modify direction if this weapon fires more than 1 bullet at the same time
					// the rand seed used to generate the direction modifier is synced across all players
					// so all bullets should be the same in other's players instances

					if (player->should_use_multiple_rand_seed())
					{
						const auto rand_vector = player->generate_bullet_rand_spread();
						const auto accuracy = 3.f * glm::radians(1.f - weapon_info->get_accuracy(false));
						const auto rotation_matrix = glm::yawPitchRoll(rand_vector.x * accuracy, rand_vector.y * accuracy, rand_vector.z * accuracy);

						direction = vec4(direction, 0.f) * rotation_matrix;
					}

					*final_muzzle_transform = Transform::look_at(muzzle, muzzle + direction);
				}
				/*else
				{
					// todojc - vehicle weapons?
				}*/
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

	// patch the loading screen lines (it's just ugly)
	// and the text tips

	jc::nop(0x4DDF97, 6);
	jc::nop(0x4DDFA0, 5);

	jc::nop(0x4DDB88, 6);
	jc::nop(0x4DDB94, 5);

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

	// weapon ai info patches (to avoid remote players from using
	// ai weapon info)

	jc::nop(0x61E529, 0xF);
	jc::nop(0x56908E, 0xF);
	jc::nop(0x61F4CB, 0x12);

	// avoid automatic destruction of vehicles after 8 seconds

	jc::nop(0x4F522E, 0x5);

	// increase max bone full update distance (by default it's 225 aka 15 meters away from camera)

	jc::write(std::pow(250.f, 2.f), 0xA56990);

	// patches vehicles get input conditional so it will try to get the input from all vehicles
	// no matter what, this helps us syncing the movement of vehicles easily without too much overkill

	jc::nop(0x850012, 8);	// for land vehicles
	jc::nop(0x8BBA29, 8);	// for sea vehicles

	// apply character's fly stance to local's patch
	// this avoids the engine setting the flying npc animation and will
	// use the player one instead

	jc::nop(0x5A3D83, 18);

	// avoid weird 2d sounds

	play_ambience_2d_sounds_hook.hook();
	fn_ret_1_hook.hook();

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

	// fire bullet sync patch

	auto fire_bullet_offset = jc::calc_call_offset(0x61F6AB + 0xD, hk_fire_bullet_patch);

	fire_bullet_patch._do(
	{
		0x8D, 0x85, 0x64, 0xFF, 0xFF, 0xFF,		// lea eax, [ebp-0x9C]
		0x50,									// push eax
		0x8B, 0x8D, 0x84, 0xFE, 0xFF, 0xFF,		// mov ecx, [ebp-0x17C]
		0xE8,									// call hook
		fire_bullet_offset.b0,
		fire_bullet_offset.b1,
		fire_bullet_offset.b2,
		fire_bullet_offset.b3,
		0xEB, 0x07								// jmp
	});
	
	// apply scoped weapons removal patch

	scope_heat_haze_patch._do(
	{
		0xEB, 0x25			// jmp
	});

	// apply weapon last muzzle transform patch

	last_muzzle_transform_patch._do(
	{
		0xEB, 0x1C			// jmp
	});

	// apply active to dead handles patch

	ai_core_dead_handles_patch._do(
	{
		0xE9, 0xE5, 0x00, 0x00, 0x00
	});

	// apply land vehicle's engine patch

	land_vehicle_engine_patch._do(
	{
		0xE9, 0xC5, 0x00, 0x00, 0x00
	});
}

void jc::patches::undo()
{
	land_vehicle_engine_patch._undo();
	ai_core_dead_handles_patch._undo();
	last_muzzle_transform_patch._undo();
	death_camera_velocity._undo();
	set_health_red_fx._undo();
	death_state._undo();
	no_idle_animation._undo();
	drop_weapon_on_death_patch._undo();
	head_rotation_patch._undo();
	fire_bullet_patch._undo();

#if FAST_LOAD
	savegame_minimal_load_patch._undo();
	savegame_integrity_patch._undo();
	info_message_patch._undo();
#endif
	
	fn_ret_1_hook.unhook();
	play_ambience_2d_sounds_hook.unhook();

	jc::write(20ui8, 0x5A4400);
}