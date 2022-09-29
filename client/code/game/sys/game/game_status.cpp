
#include <defs/standard.h>

#include "game_status.h"

#include <mp/net.h>

DEFINE_HOOK_FASTCALL(dispatch, 0x497A70, bool, GameStatus* gs)
{
	switch (gs->get_status())
	{
	case GameStatus_MainMenu:	break;
	case GameStatus_StartLoad:	break;
	case GameStatus_BeginLoad:	break;
	case GameStatus_Load:		return g_net->can_finish_load_game();	// block the loading screen until we are ready
	case GameStatus_InGame:
	{
		if (!g_net->is_joined())
			g_net->set_joined(true);

		break;
	}
	//case GameStatus_Unload: g_net->set_loaded(false); break;
	}

	return dispatch_hook(gs);
}

void GameStatus::init()
{
}

void GameStatus::destroy()
{
}

void GameStatus::hook_dispatcher()
{
	dispatch_hook.hook();
}

void GameStatus::unhook_dispatcher()
{
	dispatch_hook.unhook();
}

bool GameStatus::is_game_loaded() const
{
	return jc::read<bool>(this, jc::game_status::GAME_LOADED);
}

uint32_t GameStatus::get_status() const
{
	return jc::read<uint32_t>(this, jc::game_status::STATUS);
}