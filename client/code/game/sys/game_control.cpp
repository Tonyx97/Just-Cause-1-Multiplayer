#include <defs/standard.h>

#include "game_control.h"

#include <mp/net.h>

bool __fastcall GameControl::hk_tick(void* _this)
{
	jc::hooks::HookLock lock {};

	const auto res = jc::hooks::call<jc::proto::game_tick>(_this);

	g_net->tick();

	return res;
}

void GameControl::init()
{
}

void GameControl::destroy()
{
}

void GameControl::hook_tick()
{
	jc::hooks::hook<jc::proto::game_tick>(&hk_tick);
}

void GameControl::unhook_tick()
{
	jc::hooks::unhook<jc::proto::game_tick>();
}