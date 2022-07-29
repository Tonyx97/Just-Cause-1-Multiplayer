#include <defs/standard.h>

#include <mp/net.h>

#include "player.h"

#ifdef JC_CLIENT
#include <game/sys/factory_system.h>
#include <game/sys/world.h>
#include <game/sys/weapon_system.h>
#include <game/object/spawn_point/spawn_point.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/weapon/weapon_belt.h>
#else
#endif

#ifdef JC_CLIENT
Player::Player(PlayerClient* pc, NID nid) : client(pc)
{
	set_nid(nid);
}

void Player::verify_exec(const std::function<void(Character*)>& fn)
{
	if (local || !handle)
		return;

	if (const auto character = handle->get_character())
		fn(character);
}

void Player::respawn(float hp, float max_hp, bool sync)
{
	check(is_spawned(), "Cannot respawn a player that wasn't spawned before");

	if (sync)
	{
		check(is_local(), "Only localplayer respawning can be synced from client");

		g_net->send_reliable(PlayerPID_Respawn, hp, max_hp);
	}
	else
	{
		// update important information like position and health

		set_hp(hp);
		set_max_hp(max_hp);

		// respawn the character of this player

		get_character()->respawn();
	}
}

void Player::dispatch_movement()
{
	if (move_info.right == 0.f && move_info.forward == 0.f)
		return;

	verify_exec([&](Character* c)
	{
		// todojc - improve the bool setting

		dispatching_movement = true;
		c->dispatch_movement(move_info.angle, move_info.right, move_info.forward, move_info.aiming);
		dispatching_movement = false;
	});
}

void Player::correct_position()
{
	if (correcting_position)
	{
		const auto character = get_character();

		const auto current_position = character->get_position();

		auto interpolated_t = glm::lerp(current_position, get_position(), 0.5f);
		auto interpolated_r = glm::slerp(character->get_rotation(), get_rotation(), 0.5f);

		if (glm::distance(current_position, get_position()) < 0.05f)
			correcting_position = false;

		character->set_transform(Transform(interpolated_t, interpolated_r));
	}
}

bool Player::is_dispatching_movement() const
{
	return dispatching_movement;
}

Character* Player::get_character() const
{
	if (is_local())
		return g_world->get_localplayer_character();

	return handle ? handle->get_character() : nullptr;
}

CharacterHandle* Player::get_character_handle() const
{
	return is_local() ? nullptr : handle;
}
#else
Player::Player(PlayerClient* pc) : client(pc)
{
}

void Player::respawn(float hp, float max_hp)
{
	g_net->send_broadcast_reliable(get_client(), PlayerPID_Respawn, this, hp, max_hp);
}
#endif

Player::~Player()
{
#ifdef JC_CLIENT
	if (!is_local() && handle)
	{
		check(handle, "Invalid handle when destroying a remote player");

		set_spawned(false);

		g_factory->destroy_character_handle(handle);

		handle = nullptr;

		log(RED, "Player {} character despawned", get_nid());
	}
#else
#endif
}

bool Player::spawn()
{
	// if it's already spawned then do nothing

	if (is_spawned())
	{
		log(RED, "Player {:x} was already spawned, where are you calling this from?", get_nid());

		return false;
	}

#ifdef JC_CLIENT
	// create and spawn the character if it's not the localplayer

	if (!is_local())
	{
		handle = g_factory->spawn_character("female1");

		check(handle, "Could not create the player's character");

		log(PURPLE, "Player {:x} spawned now {:x}", get_nid(), ptr(get_character()));
	}
#endif

	set_spawned(true);

	return true;
}

// info getters/setters

void Player::set_nick(const std::string& v)
{
	dyn_info.nick = v;
}

void Player::set_skin(uint32_t v)
{
	dyn_info.skin = v;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_skin(v, false); });
#endif
}

void Player::set_hp(float v)
{
	dyn_info.hp = v;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_hp(v); });
#endif
}

void Player::set_max_hp(float v)
{
	dyn_info.max_hp = v;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_max_hp(v); });
#endif
}

void Player::set_transform(const vec3& position, const quat& rotation)
{
	dyn_info.position = position;
	dyn_info.rotation = rotation;

#ifdef JC_CLIENT
	if (!is_local())
	{
		// when receiving a transform from a remote player, we want to correct it by interpolating
		// the current transform with the one we just received

		correcting_position = true;
	}
#endif
}

void Player::set_velocity(const vec3& v)
{
	dyn_info.velocity = v;

#ifdef JC_CLIENT
	verify_exec([&](Character* c)
	{
		c->set_proxy_velocity(v);
	});
#endif
}

void Player::set_movement_angle(float angle, bool send_angle_only_next_tick)
{
	move_info.angle = angle;
	move_info.sync_angle_next_tick = send_angle_only_next_tick;
}

void Player::set_movement_info(float angle, float right, float forward, bool aiming)
{
	set_movement_angle(angle, false);

	move_info.right = right;
	move_info.forward = forward;
	move_info.aiming = aiming;
}

void Player::set_body_stance_id(uint32_t id)
{
	dyn_info.body_stance_id = id;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_body_stance(id); });
#endif
}

void Player::set_arms_stance_id(uint32_t id)
{
	dyn_info.arms_stance_id = id;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_arms_stance(id); });
#endif
}

void Player::set_head_rotation(const vec3& v, float interpolation)
{
	dyn_info.head_rotation = v;
	dyn_info.head_interpolation = interpolation;

#ifdef JC_CLIENT
	if (!is_hip_aiming() && !is_full_aiming())
		verify_exec([&](Character* c)
		{
			const auto skeleton = c->get_skeleton();

			skeleton->set_head_euler_rotation(v);
			skeleton->set_head_interpolation(interpolation);
		});
#endif
}

void Player::do_punch()
{
#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->setup_punch(); });
#endif
}

void Player::force_launch(const vec3& vel, const vec3& dir, float f1, float f2)
{
#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->force_launch(vel, dir, f1, f2); });
#endif
}

void Player::set_weapon_id(int32_t id)
{
	dyn_info.weapon_id = id;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_weapon(id); });
#endif
}

void Player::set_aim_info(bool hip, bool full, const vec3& target)
{
	dyn_info.hip_aim = hip;
	dyn_info.full_aim = full;
	dyn_info.aim_target = target;
}

void Player::fire_current_weapon(int32_t weapon_id, const vec3& muzzle, const vec3& target)
{
#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->fire_current_weapon(weapon_id, muzzle, target); });
#endif
}

void Player::reload()
{
#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->reload_current_weapon(); });
#endif
}

void Player::set_skin_info(int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props)
{
	skin_info.cloth_skin = cloth_skin;
	skin_info.head_skin = head_skin;
	skin_info.cloth_color = cloth_color;
	skin_info.props = props;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_npc_variant(cloth_skin, head_skin, cloth_color, props, false); });
#endif
}