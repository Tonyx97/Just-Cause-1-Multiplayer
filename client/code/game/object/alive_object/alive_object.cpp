#include <defs/standard.h>

#include "alive_object.h"

#include <mp/net.h>

#include <game/sys/world.h>

#include <game/object/character/character.h>

namespace jc::alive_object::hook
{
	void __fastcall set_health(AliveObject* obj, void*, float hp)
	{
		jc::hooks::HookLock lock {};

		switch (obj->get_typename_hash())
		{
		case Character::CLASS_ID():
		{
			if (const auto local_char = g_world->get_localplayer_character())
			{
				// if the character is our local one then we will send a packet to the server
				// to update and let all players know our new health, otherwise
				// if character is owned by a player then we don't want the engine
				// to remove any of his health, we will sync the health of other players
				// from packets from them or the server. if the player is local then we will
				// send a packet to the server updating our health

				if (const auto character = BITCAST(Character*, obj); character == local_char)
					g_net->send_reliable(PlayerPID_Health, hp);
				else if (g_net->get_player_by_character(character))
					return;
			}

			break;
		}
		}

		jc::hooks::call<set_health_t>(obj, hp);
	}

	void apply()
	{
		jc::hooks::hook<set_health_t>(&set_health);
	}

	void undo()
	{
		jc::hooks::unhook<set_health_t>();
	}
}

void AliveObject::set_alive_flag(uint16_t v)
{
	jc::write(get_alive_flags() | v, this, jc::alive_object::FLAGS);
}

void AliveObject::remove_alive_flag(uint16_t v)
{
	jc::write(get_alive_flags() & ~v, this, jc::alive_object::FLAGS);
}

void AliveObject::set_hp(float v)
{
	jc::hooks::call<jc::alive_object::hook::set_health_t>(this, v);
}

void AliveObject::set_max_hp(float v)
{
	jc::write(v, this, jc::alive_object::MAX_HEALTH);
}

void AliveObject::set_invincible(bool v)
{
	if (v)
		set_alive_flag(AliveObjectFlag_Invincible);
	else remove_alive_flag(AliveObjectFlag_Invincible);
}

bool AliveObject::is_alive() const
{
	return get_real_hp() > 0.f;
}

bool AliveObject::is_invincible() const
{
	return get_alive_flags() & AliveObjectFlag_Invincible;
}

uint16_t AliveObject::get_alive_flags() const
{
	return jc::read<uint16_t>(this, jc::alive_object::FLAGS);
}

float AliveObject::get_real_hp() const
{
	return jc::read<float>(this, jc::alive_object::HEALTH);
}

float AliveObject::get_max_hp() const
{
	return jc::read<float>(this, jc::alive_object::MAX_HEALTH);
}

float AliveObject::get_hp() const
{
	if (!is_alive())
		return 0.f;

	const auto current_health = get_real_hp() > get_max_hp() ? get_max_hp() : get_real_hp();

	return (current_health / get_max_hp()) * 100.f;
}