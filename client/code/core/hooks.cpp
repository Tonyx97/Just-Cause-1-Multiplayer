#include <defs/standard.h>

#include "hooks.h"

#include <game/object/character/character.h>
#include <game/object/item/item_pickup.h>

void jc::hooks::init()
{
	check(MH_Initialize() == MH_OK, "Could not initialize MinHook");
}

void jc::hooks::destroy()
{
	log(GREEN, "Destroying {} hooks...", hooks_to_destroy.size());

	for (auto target : hooks_to_destroy)
		check(MH_RemoveHook(target) == MH_OK, "Could not remove hook");

	log(GREEN, "All hooks were destroyed");

	check(MH_Uninitialize() == MH_OK, "Could not uninitialize MinHook");
}

void jc::hooks::hook_all()
{
	character::hook::apply();
	alive_object::hook::apply();
	item_pickup::hook::apply();

	check(MH_QueueEnableHook(MH_ALL_HOOKS) == MH_OK, "Could not queue all hooks");
	check(MH_EnableHook(MH_ALL_HOOKS) == MH_OK, "Could not enable all hooks");
}

void jc::hooks::unhook_all()
{
	item_pickup::hook::undo();
	alive_object::hook::undo();
	character::hook::undo();

	check(MH_QueueDisableHook(MH_ALL_HOOKS) == MH_OK, "Could not queue all hooks");
	check(MH_DisableHook(MH_ALL_HOOKS) == MH_OK, "Could not disable all hooks");
}