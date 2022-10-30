#include <defs/standard.h>

#include "alive_object.h"

#include <mp/net.h>

#include <game/sys/world/world.h>
#include <game/sys/weapon/ammo_manager.h>

#include <game/object/weapon/bullet.h>
#include <game/object/character/character.h>
#include <game/object/damageable_object/damageable_object.h>

namespace jc::alive_object::hook
{
	DEFINE_HOOK_THISCALL(set_health, 0x743E30, void, AliveObject* obj, float hp)
	{
		if (const auto localplayer = g_net->get_localplayer())
		{
			if (const auto local_char = g_world->get_local_character())
			{
				// clamp health between reasonable limits
				
				hp = std::clamp(hp, NetObject::MIN_HP(), NetObject::MAX_HP());

				switch (obj->get_typename_hash())
				{
				case Character::CLASS_ID():
				{
					// if the character is our local one then we will send a packet to the server
					// to update and let all players know our new health, otherwise
					// if character is owned by a player then we don't want the engine
					// to remove any of his health, we will sync the health of other players
					// from packets from them or the server. if the player is local then we will
					// send a packet to the server updating our health

					if (const auto character = BITCAST(Character*, obj); character == local_char)
					{
						g_net->send(Packet(WorldPID_SyncObject, ChannelID_World, localplayer, NetObjectVar_Health, hp));

						localplayer->set_hp(hp);
					}
					else if (g_net->get_player_by_character(character))
						return;

					break;
				}
				default:
				{
					if (const auto net_obj = g_net->get_net_object_by_game_object(obj))
					{
						// if this object is synced and we don't stream it, then we won't let
						// the engine damage this poor object :(
						// NOTE: in the case of damageable objects for example, when we are not streaming
						// the object and we shot at it, we rely on the streamer to replicate the shot
						// and damage the barrel on the streamer's game, that way it will tell everyone
						// that the object exploded or died

						if (!net_obj->is_owned())
							return;
					}

					break;
				}
				}
			}
		}

		set_health_hook(obj, hp);
	}

	DEFINE_INLINE_HOOK_IMPL(vehicle_inflict_damage, 0x636EF1)
	{
		// dispatch the grappling hook object attach when it's
		// the localplayer who fired the bullet
		
		if (const auto bullet = ihp->read_ebp<Bullet*>(-0x8))
			if (const auto owner = ammo_manager::g_fn::get_bullet_owner(bullet))
				if (const auto localplayer = g_net->get_localplayer(); localplayer->get_character() == owner)
					if (const auto _this = ihp->read_ebp<ObjectBase*>(0xF4); auto object = _this->get_shared())
					{
						// attach only to networked objects

						if (const auto net_obj = g_net->get_net_object_by_game_object(object.get()))
						{
							owner->set_grappled_object(object);

							localplayer->set_grappled_object(net_obj);
						}
					}
	}

	void enable(bool apply)
	{
		set_health_hook.hook(apply);
		vehicle_inflict_damage_hook.hook(apply, 0x63731E);
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
	jc::alive_object::hook::set_health_hook(this, v);
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