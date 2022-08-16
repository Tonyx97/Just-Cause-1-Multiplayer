#include <defs/standard.h>

#include <mp/net.h>

#include "player.h"

#ifdef JC_CLIENT
#include <game/sys/core/factory_system.h>
#include <game/sys/world/world.h>
#include <game/sys/weapon/weapon_system.h>
#include <game/object/spawn_point/spawn_point.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/ui/map_icon.h>

Player::Player(PlayerClient* pc, NID nid) : client(pc)
{
	set_nid(nid);
	set_sync_type(SyncType_Locked);
}

Player::~Player()
{
	if (!is_local() && handle)
	{
		check(handle, "Invalid handle when destroying a remote player");

		set_spawned(false);

		g_factory->destroy_character_handle(handle);
		g_factory->destroy_map_icon(blip);

		handle = nullptr;

		log(RED, "Player {} character despawned", get_nid());
	}
}

ObjectBase* Player::get_object_base()
{
	return get_character();
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

void Player::update_blip()
{
	blip->set_position(get_position());
}

void Player::set_multiple_rand_seed(uint16_t v)
{
	dyn_info.multiple_bullet_mt.seed(v);
	dyn_info.use_multiple_bullet = (v != 0u);
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

vec3 Player::generate_bullet_rand_spread()
{
	return
	{
		util::rand::rand_flt(dyn_info.multiple_bullet_mt, -1.f, 1.f),
		util::rand::rand_flt(dyn_info.multiple_bullet_mt, -1.f, 1.f),
		util::rand::rand_flt(dyn_info.multiple_bullet_mt, -1.f, 1.f)
	};
}
#else
Player::Player(PlayerClient* pc) : client(pc)
{
	set_sync_type(SyncType_Locked);
	set_streamer(this);
}

Player::~Player()
{
	// if this player owns any net object, remove all the ownerships
	// from the player and the objects

	remove_all_ownerships();
}

void Player::respawn(float hp, float max_hp)
{
	g_net->send_broadcast_reliable(get_client(), PlayerPID_Respawn, this, hp, max_hp);
}

void Player::transfer_net_object_ownership_to(NetObject* obj, Player* new_streamer)
{
	check(obj, "Net object must be valid");
	check(new_streamer, "New streamer must be valid");

	const auto old_streamer_pc = client;
	const auto new_streamer_pc = new_streamer->get_client();

	old_streamer_pc->send_reliable<ChannelID_World>(WorldPID_SetOwnership, new_streamer, obj);
	new_streamer_pc->send_reliable<ChannelID_World>(WorldPID_SetOwnership, new_streamer, obj);
}

void Player::set_net_object_ownership_of(NetObject* obj)
{
	check(obj, "Net object must be valid");

	client->send_reliable<ChannelID_World>(WorldPID_SetOwnership, this, obj);
}

void Player::remove_all_ownerships()
{
	g_net->for_each_net_object([&](NID, NetObject* obj)
	{
		if (obj->is_owned_by(this))
			obj->set_streamer(nullptr);
	});
}
#endif

void Player::on_net_var_change(NetObjectVarType var_type)
{
#ifdef JC_CLIENT
	switch (var_type)
	{
	case NetObjectVar_Transform:
	case NetObjectVar_Position:
	case NetObjectVar_Rotation:
	{
		if (!is_local())
		{
			// when receiving a transform from a remote player, we want to correct it by interpolating
			// the current transform with the one we just received

			correcting_position = true;
		}

		break;
	}
	case NetObjectVar_Velocity: verify_exec([&](Character* c) { c->set_added_velocity(get_velocity()); }); break;
	case NetObjectVar_Health: verify_exec([&](Character* c) { c->set_hp(get_hp()); }); break;
	case NetObjectVar_MaxHealth: verify_exec([&](Character* c) { c->set_max_hp(get_max_hp()); }); break;
	}
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

		blip = g_factory->create_map_icon("player_blip", get_position());

		check(blip, "Could not create the player's blip");
	}
	else
	{
		// setup our localplayer when spawning for the first time

		const auto character = get_character();

		character->set_skin(0, false);
		character->set_hp(500.f);
		character->set_max_hp(500.f);
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

void Player::set_skin(int32_t v)
{
	dyn_info.skin = v;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_skin(v, false); });
#endif
}

void Player::set_skin(int32_t v, int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props)
{
	set_skin(v);
	set_skin_info(cloth_skin, head_skin, cloth_color, props);

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_skin(v, cloth_skin, head_skin, cloth_color, props, false); });
#endif
}

void Player::set_walking_set_and_skin(int32_t walking_set_id, int32_t skin_id)
{
	dyn_info.walking_set = walking_set_id;
	dyn_info.skin = skin_id;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_walking_anim_set(walking_set_id, skin_id, false); });
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
	const bool force_sync_angle = move_info.angle != angle && (move_info.right != 0.f || move_info.forward != 0.f);

	move_info.force_sync = move_info.right != right || move_info.forward != forward || move_info.aiming != aiming;

	set_movement_angle(angle, !move_info.force_sync && force_sync_angle);

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
	if (dyn_info.weapon_id == id)
		return;
	
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

void Player::set_bullet_direction(const vec3& muzzle, const vec3& dir)
{
	dyn_info.muzzle_position = muzzle;
	dyn_info.bullet_dir = dir;
	dyn_info.aim_target = muzzle + dir;
}

void Player::fire_current_weapon(int32_t weapon_id, const vec3& muzzle, const vec3& dir)
{
	set_bullet_direction(muzzle, dir);

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->fire_current_weapon(weapon_id, muzzle, dyn_info.aim_target); });
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