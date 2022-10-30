#include <defs/standard.h>

#include "bullet.h"

#include <game/object/character/character.h>
#include <game/object/weapon/weapon.h>
#include <game/object/vars/weapons.h>
#include <game/object/game_player/game_player.h>

#include <game/sys/weapon/ammo_manager.h>
#include <game/sys/resource/physics.h>

#include <mp/net.h>

namespace jc::bullet::hook
{
	DEFINE_HOOK_THISCALL(step, 0x6466F0, void, Bullet* bullet, float delta)
	{
		step_hook(bullet, delta);

		// for some reason, the game won't destroy bullets that hit very very far
		// away so we will just check the velocity here and kill them ourselves

		if (glm::length(bullet->get_velocity()) <= 5.f)
			bullet->set_pending_kill();

		// custom bullet drop calculation for bullets
		// and overall, bullets that are usually affected
		// by gravity

		switch (const auto type = bullet->get_type())
		{
		case BulletType_Small:
		case BulletType_Medium:
		case BulletType_Large:
		case BulletType_ShotgunOrMG:
		case BulletType_Grenade:
		case BulletType_Hook:
		{
			const auto gravity = g_physics->get_gravity();
			const auto direction = bullet->get_direction();
			const auto velocity = bullet->get_velocity();

			const auto gravity_modifier = 2.5f;

			vec3 new_direction =
			{
				(direction.x * velocity) + gravity.x * gravity_modifier * delta,
				(direction.y * velocity) + gravity.y * gravity_modifier * delta,
				(direction.z * velocity) + gravity.z * gravity_modifier * delta
			};

			bullet->set_velocity(glm::length(new_direction));
			bullet->set_direction(new_direction);

			// render the grapple hook cable while in air for
			// remote players
			
			if (type == BulletType_Hook)
				if (const auto owner = ammo_manager::g_fn::get_bullet_owner(bullet))
					if (const auto weapon = owner->get_current_weapon())
					{
						if (const auto player = g_net->get_player_by_character(owner); player != g_net->get_localplayer())
							player->get_game_player()->draw_grappling_hook(weapon->get_muzzle_position(), bullet->get_position(), true);
						else if (player)
						{
							// if it's the localplayer bullet then update the grappling position so it
							// shows the trashy white line
							
							jc::write(bullet->get_position(), 0xD86024);
						}
					}

			break;
		}
		}
	}

	DEFINE_HOOK_THISCALL(hit_object, 0x64FFB0, void, Bullet* bullet, vec3* hit_pos, ObjectBase** object_ptr)
	{
		hit_object_hook(bullet, hit_pos, object_ptr);
	}

	DEFINE_INLINE_HOOK_IMPL(update_grappling_hook_relative_position, 0x6479A2)
	{
		const auto bullet = ihp->read_ebp<Bullet*>(0xC0);

		// sends our grappling attach info to all players
		
		if (const auto owner = ammo_manager::g_fn::get_bullet_owner(bullet); owner && owner == g_net->get_localplayer()->get_character())
			if (auto object = owner->get_grappled_object().lock())
				if (const auto net_obj = g_net->get_net_object_by_game_object(object.get()))
				{
					const auto relative_pos = ihp->at_ebp<vec3>(0x88);

					g_net->send(Packet(PlayerPID_GrapplingHookAttachDetach, ChannelID_Generic, true, net_obj, *relative_pos));
				}
	}

	void enable(bool apply)
	{
		step_hook.hook(apply);
		hit_object_hook.hook(apply);
		update_grappling_hook_relative_position_hook.hook(apply);
	}
}

void Bullet::calculate_drop(float delta)
{
	jc::v_call(this, jc::bullet::vt::CALCULATE_DROP, delta);
}

void Bullet::set_position(const vec3& v)
{
	jc::write(v, this, jc::bullet::POSITION);
}

void Bullet::set_velocity(float v)
{
	jc::write(jc::game::encode_float(v), this, jc::bullet::VELOCITY);
}

void Bullet::set_direction(const vec3& v)
{
	jc::write(glm::normalize(v), this, jc::bullet::DIRECTION);
}

void Bullet::set_pending_kill()
{
	jc::write(false, this, jc::bullet::IS_ALIVE);
}

bool Bullet::is_alive()
{
	return jc::read<bool>(this, jc::bullet::IS_ALIVE);
}

uint16_t Bullet::get_type()
{
	return jc::read<uint16_t>(this, jc::bullet::TYPE);
}

float Bullet::get_damage()
{
	return jc::read<float>(this, jc::bullet::DAMAGE);
}

float Bullet::get_max_range()
{
	return jc::read<float>(this, jc::bullet::MAX_RANGE);
}

float Bullet::get_velocity()
{
	return jc::game::decode_float(jc::read<uint16_t>(this, jc::bullet::VELOCITY));
}

vec3 Bullet::get_position()
{
	return jc::read<vec3>(this, jc::bullet::POSITION);
}

vec3 Bullet::get_direction()
{
	return jc::read<vec3>(this, jc::bullet::DIRECTION);
}

vec3 Bullet::get_last_position()
{
	return jc::read<vec3>(this, jc::bullet::LAST_POSITION);
}