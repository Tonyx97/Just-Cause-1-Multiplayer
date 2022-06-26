#include <defs/standard.h>

#include "game_status.h"

#include <mp/net.h>

bool __fastcall GameStatus::hk_dispatch(GameStatus* gs)
{
	jc::hooks::HookLock lock {};
	
	auto res = jc::hooks::call<jc::proto::game_status::dispatch>(gs);

	switch (gs->get_status())
	{
	case GameStatus_MainMenu: break;
	case GameStatus_StartLoad: break;
	case GameStatus_BeginLoad: break;
	case GameStatus_Load: break;
	case GameStatus_InGame:
	{
		if (g_net->is_initialized() && !g_net->is_loaded())
			g_net->set_state(PCState_Loaded);

		break;
	}
	//case GameStatus_Unload: g_net->set_loaded(false); break;
	}

	return res;
}

void GameStatus::init()
{
}

void GameStatus::destroy()
{
}

void GameStatus::hook_dispatcher()
{
	jc::hooks::hook<jc::proto::game_status::dispatch>(&hk_dispatch);
}

void GameStatus::unhook_dispatcher()
{
	jc::hooks::unhook<jc::proto::game_status::dispatch>();
}

bool GameStatus::is_game_loaded() const
{
	return jc::read<bool>(this, jc::game_status::GAME_LOADED);
}

uint32_t GameStatus::get_status() const
{
	return jc::read<uint32_t>(this, jc::game_status::STATUS);
}