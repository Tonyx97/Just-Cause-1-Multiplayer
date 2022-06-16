#include <defs/standard.h>

#include "game_status.h"

void GameStatus::init()
{
}

void GameStatus::destroy()
{
}

uint32_t GameStatus::get_status() const
{
	return jc::read<uint32_t>(this, jc::game_status::STATUS);
}