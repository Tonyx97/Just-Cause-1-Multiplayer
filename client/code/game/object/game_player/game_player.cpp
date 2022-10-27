#include <defs/standard.h>

#include "game_player.h"

#include "../character/character.h"
#include "../camera/camera.h"
#include "../parachute/parachute.h"

#include <mp/net.h>

#include <game/sys/world/world.h>
#include <game/sys/camera/camera_manager.h>

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

	DEFINE_INLINE_HOOK_IMPL(close_parachute, 0x4C67D8)
	{
		if (const auto character = ihp->read_ebp<Character*>(0x334))
			if (const auto local_char = g_world->get_local_character(); local_char && local_char == character)
				g_net->send(Packet(PlayerPID_ParachuteControl, ChannelID_Generic, 0ui8));
	}

	DEFINE_INLINE_HOOK_IMPL(open_parachute, 0x603711)
	{
		if (const auto character = ihp->read_ebp<Character*>(0x94))
			if (const auto local_char = g_world->get_local_character(); local_char && local_char == character)
				g_net->send(Packet(PlayerPID_ParachuteControl, ChannelID_Generic, 1ui8));
	}
	
	DEFINE_HOOK_THISCALL_S(update, fn::UPDATE, void, GamePlayer* game_player)
	{
		const auto local_gp = g_world->get_local();

		// local player
		
		if (local_gp == game_player)
		{
			update_hook(game_player);

			const auto localplayer = g_net->get_localplayer();
			if (!localplayer)
				return;

			const auto camera = jc::this_call(0x582920, jc::read<ptr>(0xD8509C), 1);
			if (!camera)
				return;

			const auto cam_yaw = jc::v_call<float>(camera, 4);
			const bool is_looking = jc::read<int8_t>(game_player, 0x13D) || jc::read<int8_t>(game_player, 0x13E);

			localplayer->set_movement_info(cam_yaw, local_gp->get_right(), local_gp->get_forward(), is_looking);

			return;
		}

		// remote players
		
		const auto player_char = game_player->get_character();

		check(player_char, "Trying to update a GamePlayer with no character");

		if (const auto player = g_net->get_player_by_character(player_char))
		{
			const auto& move_info = player->get_movement_info();

			// update the variables for the remote player's GamePlayer instance
			
			game_player->set_right(move_info.right);
			game_player->set_forward(move_info.forward);

			//log(GREEN, "{} {}", move_info.right, move_info.forward);

			auto state_id = jc::read<int>(game_player, 0x130);

			check(jc::read<int>(game_player, 0x134) == 0, "Not implemented 0");

			jc::write(true, game_player, 0x1D8); // seems to block the key input

			if (!jc::this_call<bool>(0x597E30, player_char) || jc::this_call<bool>(0x597E80, player_char))
			{
				if (jc::this_call<bool>(0x597E80, player_char) || jc::this_call<bool>(0x5A2080, player_char))
				{
					//check(false, "Not implemented 1");
				}
				else
				{
					if (jc::this_call(0x597B00, player_char))
					{
						check(false, "Not implemented 2");
					}
					else
					{
						if (!player_char->is_in_vehicle() || !jc::this_call<bool>(0x58F340, player_char, 0))
						{
							if (jc::this_call<bool>(0x596420, player_char) || jc::this_call<bool>(0x596550, player_char))
							{
								jc::this_call(0x4C8470, game_player);
							}
							else
							{
								if (state_id == 5)
								{
									check(false, "Not implemented 3");
								}

								//state_id = 4;

								state_id = 2;

								switch (state_id)
								{
								case 2:
								{
									jc::this_call(0x5A45D0, player_char, move_info.angle, move_info.right, move_info.forward, move_info.aiming);
									break;
								}
								}
							}
						}
					}
				}
			}

			jc::this_call(0x4CB710, game_player);
		}
	}

	void enable(bool apply)
	{
		switch_to_previous_weapon_hook.hook(apply);
		switch_to_next_weapon_hook.hook(apply);
		close_parachute_hook.hook(apply);
		open_parachute_hook.hook(apply);
		update_hook.hook(apply);
	}
}

GamePlayer* GamePlayer::CREATE()
{
	const auto player = jc::game::malloc<GamePlayer>(0x53C);

	jc::this_call(jc::game_player::fn::CTOR, player);

	// load the parachute model for this GamePlayer
	
	if (player)
		player->load_parachute_model();

	return player;
}

void GamePlayer::destroy()
{
	jc::v_call(this, 0, 1);
}

void GamePlayer::load_parachute_model()
{
	// this will only work if the instance has a linked character
	
	if (const auto parachute = get_parachute())
		parachute->load_model();
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

Parachute* GamePlayer::get_parachute() const
{
	return REF(Parachute*, this, jc::game_player::PARACHUTE);
}