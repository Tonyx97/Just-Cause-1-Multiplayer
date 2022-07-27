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
	set_player_client(pc);
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
	verify_exec([&](Character* c)
	{
		// todojc - improve the bool setting 

		dispatching_movement = true;
		c->dispatch_movement(move_info.angle, move_info.right, move_info.forward, move_info.aiming);
		dispatching_movement = !is_alive();
	});
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
#else
Player::Player(PlayerClient* pc) : client(pc)
{
	set_player_client(pc);
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
		handle = g_factory->spawn_character("female1", { 3393.f, 180.04f, 8997.96f });

		check(handle, "Could not create the player's character");

		log(PURPLE, "Player {:x} spawned now", get_nid());
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

void Player::set_transform(const Transform& transform)
{
	dyn_info.transform = transform;
}

void Player::set_movement_info(float angle, float right, float forward, bool aiming)
{
	move_info.angle = angle;
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

void Player::set_fire_weapon_info(bool fire, int32_t weapon_id, const vec3& muzzle, const vec3& target)
{
	dyn_info.firing_weapon_id = weapon_id;
	dyn_info.fire_weapon = fire;
	dyn_info.fire_muzzle = muzzle;
	dyn_info.fire_target = target;
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