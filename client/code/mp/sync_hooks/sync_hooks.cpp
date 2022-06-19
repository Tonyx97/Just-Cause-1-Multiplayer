#include <defs/standard.h>

#include "sync_hooks.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/sys/all.h>

#include <game/object/character/character.h>

void jc::sync_hooks::apply()
{
	character::hook::apply();
}

void jc::sync_hooks::undo()
{
	character::hook::undo();
}