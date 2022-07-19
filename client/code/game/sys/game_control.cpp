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

void GameControl::on_tick()
{
	// debug test units

	jc::test_units::test_0();

	// do mod logic

	g_net->tick();
	g_net->update_objects();
	g_key->clear_states();

	timer::dispatch();
}