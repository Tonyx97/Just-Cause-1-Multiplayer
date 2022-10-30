#include <defs/standard.h>

#include "hooks.h"

#include <game/object/game_player/game_player.h>
#include <game/object/character/character.h>
#include <game/object/item/item_pickup.h>
#include <game/object/interactable/interactable.h>
#include <game/object/vehicle/comps/vehicle_seat.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/physics/pfx_collision.h>
#include <game/object/weapon/bullet.h>
#include <game/object/weapon/weapon.h>
#include <game/object/camera/cam_settings.h>

#include <game/sys/all.h>

void jc::hooks::init()
{
	check(MH_Initialize() == MH_OK, "Could not initialize MinHook");
}

void jc::hooks::destroy()
{
	check(MH_Uninitialize() == MH_OK, "Could not uninitialize MinHook");
}

void jc::hooks::hook_queued()
{
	const auto tid = uint32_t(GetCurrentThreadId());

	auto& pending_hooks_list = pending_hooks[tid];
	auto& placed_hooks_list = placed_hooks[tid];

	for (auto target : pending_hooks_list)
		check(MH_QueueEnableHook(target) == MH_OK, "Could not queue all hooks");

	check(MH_EnableHook(MH_ALL_HOOKS) == MH_OK, "Could not enable all hooks");
	
	for (auto target : pending_hooks_list)
		placed_hooks_list.push_back(target);

	log(CYAN, "Hooked {} functions in thread {:x}", pending_hooks_list.size(), tid);

	pending_hooks_list.clear();
}

void jc::hooks::unhook_queued()
{
	const auto tid = uint32_t(GetCurrentThreadId());

	auto& placed_hooks_list = placed_hooks[tid];

	for (auto target : placed_hooks_list)
		check(MH_QueueDisableHook(target) == MH_OK, "Could not queue all hooks");

	check(MH_DisableHook(MH_ALL_HOOKS) == MH_OK, "Could not disable all hooks");

	for (auto target : placed_hooks_list)
		check(MH_RemoveHook(target) == MH_OK, "Could not remove hook");

	log(CYAN, "Unhooked {} functions in thread {:x}", placed_hooks_list.size(), tid);

	placed_hooks_list.clear();

	log(GREEN, "All hooks were destroyed");
}

void jc::hooks::hook_game_fns(bool enable)
{
	ammo_manager::hook::enable(enable);
	game_player::hook::enable(enable);
	character::hook::enable(enable);
	alive_object::hook::enable(enable);
	item_pickup::hook::enable(enable);
	pfx_collision::hook::enable(enable);
	interactable::hook::enable(enable);
	vehicle_seat::hook::enable(enable);
	vehicle::hook::enable(enable);
	weapon::hook::enable(enable);
	bullet::hook::enable(enable);
	physics::hook::enable(enable);
	cam_settings::hook::enable(enable);
}