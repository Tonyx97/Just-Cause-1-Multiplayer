#include <defs/standard.h>

#include "game_player.h"

#include "../character/character.h"

#include <mp/net.h>

namespace jc::game_player::hook
{
	DEFINE_HOOK_THISCALL(switch_to_previous_weapon, 0x4CDD80, void, GamePlayer* player, float delta)
	{
		player->increase_current_weapon_scope_fov(0.3f);
	}

	DEFINE_HOOK_THISCALL(switch_to_next_weapon, 0x4CDDF0, void, GamePlayer* player, float delta)
	{
		player->decrease_current_weapon_scope_fov(0.3f);
	}

	void enable(bool apply)
	{
		switch_to_previous_weapon_hook.hook(apply);
		switch_to_next_weapon_hook.hook(apply);
	}
}

void GamePlayer::reset_weapon_belt()
{
	jc::this_call(jc::game_player::fn::RESET_WEAPON_BELT, this);
}

void GamePlayer::teleport_to_closest_safehouse()
{
	jc::this_call(jc::game_player::fn::TELEPORT_TO_CLOSEST_SAFEHOUSE, this);
}

void GamePlayer::increase_current_weapon_scope_fov(float factor)
{
	jc::this_call(jc::game_player::fn::INCREASE_CURRENT_WEAPON_SCOPE_FOV, this, factor);
}

void GamePlayer::decrease_current_weapon_scope_fov(float factor)
{
	jc::this_call(jc::game_player::fn::DECREASE_CURRENT_WEAPON_SCOPE_FOV, this, factor);
}

void GamePlayer::set_right(float v)
{
	jc::write(v, this, jc::game_player::RIGHT);
}

void GamePlayer::set_forward(float v)
{
	jc::write(v, this, jc::game_player::FORWARD);
}

float GamePlayer::get_right() const
{
	return jc::read<float>(this, jc::game_player::RIGHT);
}

float GamePlayer::get_forward() const
{
	return jc::read<float>(this, jc::game_player::FORWARD);
}

Character* GamePlayer::get_character() const
{
	return jc::read<Character*>(this, jc::game_player::CHARACTER);
}