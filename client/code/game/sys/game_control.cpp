#include <defs/standard.h>

#include "game_control.h"

#include <timer/timer.h>

#include <mp/net.h>

#include <game/object/character/character.h>
#include <game/object/localplayer/localplayer.h>
#include <game/object/character_handle/character_handle.h>
#include <game/sys/world.h>
#include <game/sys/factory_system.h>
#include <core/keycode.h>
#include <core/test_units.h>

void GameControl::init()
{
}

void GameControl::destroy()
{
}

void GameControl::hook_tick()
{
	//tick_hook.hook();
}

void GameControl::unhook_tick()
{
	//tick_hook.unhook();
}