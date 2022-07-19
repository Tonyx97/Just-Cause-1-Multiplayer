#include <defs/standard.h>

#include <mp/net.h>

#include "player.h"

#ifdef JC_CLIENT
#include <game/sys/factory_system.h>
#include <game/sys/world.h>
#include <game/object/spawn_point/spawn_point.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#else
#endif

#ifdef JC_CLIENT
Player::Player(PlayerClient* pc, NID nid) : client(pc)
{
	set_nid(nid);
	set_player_client(pc);

	if (const auto localplayer = g_net->get_localplayer())
	{
		if (!localplayer->equal(this))
		{
			// create and spawn the character if it's not the localplayer

			handle = g_factory->spawn_character("female1", { 3393.f, 180.04f, 8997.96f });

			set_spawned(true);

			log(PURPLE, "Player {:x} spawned now", get_nid());
		}
	}
}

void Player::verify_exec(const std::function<void(Character*)>& fn)
{
	if (local || !handle)
		return;

	if (const auto character = handle->get_character())
		fn(character);
}

Character* Player::get_character() const
{
	if (is_local())
		return g_world->get_localplayer_character();

	return handle->get_character();
}
#else
Player::Player(PlayerClient* pc) : client(pc)
{
	set_player_client(pc);
}
#endif

Player::~Player()
{
#ifdef JC_CLIENT
	if (!is_local() && handle)
	{
		check(handle, "Invalid agent spawn point when destroying a remote player");

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
		return false;

#ifdef JC_CLIENT
	// create agent spawn point if this player is not a local one

	log(GREEN, "Sync spawning for player {:x} {}", get_nid(), get_nick());
#else
	g_net->send_broadcast_reliable<ChannelID_Generic>(get_client(), PlayerPID_Spawn, this);

	set_spawned(true);
#endif

	return true;
}

// info getters/setters

void Player::set_hp(float v)
{
#ifdef JC_CLIENT
	verify_exec([&](Character* c)
	{
		// todojc - probably not a good idea to do this here but it's fine for now

		if (!is_alive() && v > 0.f)
			c->respawn();

		c->set_hp(v);
	});
#endif

	dyn_info.hp = v;
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

void Player::set_head_rotation(const vec3& v)
{
	dyn_info.head_rotation = v;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->get_skeleton()->set_head_euler_rotation(v); });
#endif
}

void Player::do_punch()
{
#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->setup_punch(); });
#endif
}

// static info getters/setters

void Player::set_nick(const std::string& v)
{
	static_info.nick = v;
}

void Player::set_skin(uint32_t v)
{
	static_info.skin = v;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_model(v, false); });
#endif
}

uint32_t Player::get_skin() const
{
	return static_info.skin;
}

bool Player::must_skip_engine_stances() const
{
	return skip_engine_stances;
}