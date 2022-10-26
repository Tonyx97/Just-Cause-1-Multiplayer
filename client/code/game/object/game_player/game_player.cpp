#include <defs/standard.h>

#include "game_player.h"

#include "../character/character.h"

#include <mp/net.h>

#include <game/sys/world/world.h>

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

	DEFINE_HOOK_THISCALL_S(update, fn::UPDATE, void, GamePlayer* game_player)
	{
		if (g_world->get_local() == game_player)
			update_hook(game_player);
		else
		{
			const auto player_char = game_player->get_character();

			check(player_char, "Trying to update a GamePlayer with no character");

			if (const auto player = g_net->get_net_object_by_game_object(player_char)->cast<Player>())
			{
				player->dispatch_movement();

				//jc::this_call(0x4C7860, game_player);
			}

			// if it's not climbing a ladder then update
			// the movement, otherwise update the ladder movement

			/*if (!player_char->is_climbing_ladder())
				player->dispatch_movement();
			else
			{
				// dispatch ladder movement
				// todojc - clean this shit

				//log(GREEN, "{}", forward);

				const auto forward = move_info.forward;

				if (forward >= 0.f)
				{
					if (forward <= 0.f)
					{
						jc::this_call(0x59A280, player_char, false);
					}
					else
					{
						jc::this_call(0x59F640, player_char, 1.f);

						if (jc::this_call<bool>(0x597A60, player_char))
							jc::this_call(0x59A280, player_char, true);
					}
				}
				else
				{
					jc::this_call(0x59F6B0, player_char, 1.f);

					if (jc::this_call<bool>(0x597A90, player_char))
						jc::this_call(0x59A280, player_char, true);
				}
			}*/
		}
	}

	void enable(bool apply)
	{
		switch_to_previous_weapon_hook.hook(apply);
		switch_to_next_weapon_hook.hook(apply);
		update_hook.hook(apply);
	}
}

GamePlayer* GamePlayer::CREATE()
{
	const auto player = jc::game::malloc<GamePlayer>(0x53C);

	jc::this_call(jc::game_player::fn::CTOR, player);

	return player;
}

void GamePlayer::destroy()
{
	jc::v_call(this, 0, 1);
}

void GamePlayer::update()
{
	// call the hook and filter there

	jc::this_call(jc::game_player::fn::UPDATE, this);
}

void GamePlayer::set_character(Character* character)
{
	jc::write(character, this, jc::game_player::CHARACTER);
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