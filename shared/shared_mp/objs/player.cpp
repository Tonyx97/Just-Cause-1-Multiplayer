#include <defs/standard.h>

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

#include "player.h"

#include <resource_sys/resource_system.h>

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
}

Player::~Player()
{
	if (is_local())
		return;

	destroy_object();
}

ObjectBase* Player::get_object_base() const
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
	if (vehicle)
		set_position(vehicle->get_position());

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
#include <rg/rg.h>

Player::Player(PlayerClient* pc) : client(pc)
{
	set_sync_type(SyncType_Locked);
}

Player::~Player()
{
	// remove this player from the current vehicle if any

	if (vehicle)
		vehicle->remove_player(this);
}

void Player::send_ownerships()
{
	if (std::vector<NID> owned_entities; get_rg()->get_owned_entities(owned_entities))
		client->send(Packet(PlayerClientPID_Ownerships, ChannelID_PlayerClient, owned_entities), true);
}
#endif

void Player::destroy_object()
{
#ifdef JC_CLIENT
	if (handle) g_factory->destroy_character_handle(std::exchange(handle, nullptr));
	if (blip)	g_factory->destroy_map_icon(std::exchange(blip, nullptr));
#endif
}

void Player::on_spawn()
{
#ifdef JC_CLIENT
	if (is_local())
	{
		// setup our localplayer when spawning for the first time

		const auto character = get_character();

		character->set_skin(0, false);
		character->set_hp(500.f);
		character->set_max_hp(500.f);
	}
	else
	{
		// create and spawn the character if it's not the localplayer

		handle = g_factory->spawn_character("female1");

		check(handle, "Could not create the player's character");

		log(PURPLE, "Player {:x} spawned now {:x} - handle {:x}", get_nid(), ptr(get_character()), ptr(handle));

		blip = g_factory->create_map_icon("player_blip", get_position());

		check(blip, "Could not create the player's blip");
	}
#endif
}

void Player::on_despawn()
{
	destroy_object();
}

void Player::on_net_var_change(NetObjectVarType var_type)
{
	switch (var_type)
	{
	case NetObjectVar_Transform:
	case NetObjectVar_Position:
	case NetObjectVar_Rotation:
	{
		// if player is in vehicle we don't need to update the player transform

#ifdef JC_CLIENT
		if (!vehicle)
		{
			if (!is_local())
			{
				// when receiving a transform from a remote player, we want to correct it by interpolating
				// the current transform with the one we just received

				correcting_position = true;
			}
		}
#endif

		break;
	}
	case NetObjectVar_Velocity:
	{
#ifdef JC_CLIENT
		verify_exec([&](Character* c) { c->set_added_velocity(get_velocity()); });
#endif
		break;
	}
	case NetObjectVar_Health:
	{
#ifdef JC_CLIENT
		verify_exec([&](Character* c) { c->set_hp(get_hp()); });
#endif

		// if old health was bigger than 0 but new one is 0
		// it means we died so trigger killed event, if the player
		// was just revived, make sure we respawn it properly
		
		if (was_just_killed())
			g_rsrc->trigger_event(jc::script::event::ON_PLAYER_KILLED, this);
		else if (was_just_revived())
			respawn();

		break;
	}
	case NetObjectVar_MaxHealth:
	{
#ifdef JC_CLIENT
		verify_exec([&](Character* c) { c->set_max_hp(get_max_hp()); });
#endif
		break;
	}
	}
}

void Player::respawn(const vec3& position, float rotation, int32_t skin, float hp, float max_hp)
{
	if (!is_spawned())
		return;

	const auto angles = glm::eulerAngleXYZ(0.f, rotation, 0.f);

#ifdef JC_CLIENT
	const auto character = get_character();
	if (!character)
		return;

	// respawn the character of this player

	respawn();

	// if it's local we want to access directly to the
	// character and set the stuff, verify_exec doesn't work
	// with localplayer due to safety reasons so we have to access
	// the local's character

	if (is_local())
	{
		character->set_transform(Transform(position, angles));
		character->set_skin(skin, false);
		character->set_hp(hp);
		character->set_max_hp(max_hp);

		return;
	}
#else

	Packet p(PlayerPID_Respawn, ChannelID_Generic, this, position, rotation, skin, hp, max_hp);

	g_net->send_broadcast(p);
#endif

	set_transform(TransformTR(position, angles));
	set_skin(skin);
	set_hp(hp);
	set_max_hp(max_hp);
}

void Player::respawn()
{
	if (!is_spawned())
		return;

#ifdef JC_CLIENT
	const auto character = get_character();
	if (!character)
		return;

	character->respawn();
#endif
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

void Player::set_vehicle(uint8_t seat_type, VehicleNetObject* v)
{
	// remove the player from the current vehicle (if any)

	if (vehicle)
	{
		vehicle->remove_player(this);
		vehicle = nullptr;
		vehicle_seat = VehicleSeat_None;
	}

	// if the new vehicle is valid then add the player to it

	if (v)
		v->set_player(seat_type, this);

	//log(RED, "{}'s vehicle: {:x}", get_nick(), v ? v->get_nid() : INVALID_NID);

	vehicle = v;
	vehicle_seat = seat_type;
}