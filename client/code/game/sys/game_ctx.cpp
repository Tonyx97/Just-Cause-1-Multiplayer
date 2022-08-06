#include <defs/standard.h>

#include "game_ctx.h"

void GameContext::init()
{
}

void GameContext::destroy()
{
}

void GameContext::set_window_resolution(int32_t x, int32_t y)
{
	jc::write(x, 0x403679); // initial x
	jc::write(y, 0x403680); // initial y
	jc::write(x, this, jc::game_ctx::SCREEN_SIZE_X);
	jc::write(y, this, jc::game_ctx::SCREEN_SIZE_Y);
	jc::write(0x15, this, jc::game_ctx::UNKNOWN);
}

void GameContext::set_fullscreen(bool enabled)
{
	jc::write(enabled, 0xAF1E08);
}