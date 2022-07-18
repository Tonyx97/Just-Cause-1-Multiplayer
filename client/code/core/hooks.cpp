#include <defs/standard.h>

#include "hooks.h"

#include <game/object/character/character.h>
#include <game/object/item/item_pickup.h>

void jc::hooks::hook_all()
{
	character::hook::apply();
	alive_object::hook::apply();
	item_pickup::hook::apply();

	check(MH_EnableHook(MH_ALL_HOOKS) == MH_OK, "Could not enable all hooks");
}

void jc::hooks::unhook_all()
{
	item_pickup::hook::undo();
	alive_object::hook::undo();
	character::hook::undo();

	check(MH_DisableHook(MH_ALL_HOOKS) == MH_OK, "Could not disable all hooks");
}