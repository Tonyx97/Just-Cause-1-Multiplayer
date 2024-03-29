#include <defs/standard.h>

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

#include "player.h"

#include <resource_sys/resource_system.h>

#ifdef JC_CLIENT
#include <game/sys/core/factory_system.h>
#include <game/sys/world/world.h>
#include <game/sys/weapon/ammo_manager.h>
#include <game/sys/weapon/weapon_system.h>
#include <game/object/parachute/parachute.h>
#include <game/object/spawn_point/spawn_point.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/ui/map_icon.h>
#include <game/object/game_player/game_player.h>

Player::Player(PlayerClient* pc, NID nid) : client(pc)
{
	set_nid(nid);
}

Player::~Player()
{
	// remove instance from the vehicle

	set_vehicle(VehicleSeat_None, nullptr);

	// if it's local we don't have to do anything else

	if (is_local())
		return;

	// do not destroy the local's GamePlayer

	if (game_player)
	{
		game_player->set_character(nullptr);
		game_player->destroy();
		game_player = nullptr;
	}
	
	// destroy the player character etc
	
	destroy_object();
}

Character* Player::get_character() const
{
	if (is_local())
		return g_world->get_local_character();

	return handle ? handle->get_character() : nullptr;
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

void Player::update_game_player()
{
	if (game_player)
		game_player->update();
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

	if (blip)
		blip->set_position(get_position());
}

void Player::set_multiple_rand_seed(uint16_t v)
{
	dyn_info.multiple_bullet_mt.seed(v);
	dyn_info.use_multiple_bullet = (v != 0u);
}

void Player::respawn_character()
{
	if (!is_spawned())
		return;

#ifdef JC_CLIENT
	const auto character = get_character();
	if (!character)
		return;

	character->respawn();
	character->set_body_stance(0);
	character->set_arms_stance(0);
#endif
}

bool Player::is_dispatching_movement() const
{
	return dispatching_movement;
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
	// clear our custom map if the character is going to be destroyed

	jc::ammo_manager::g_fn::clear_owner_bullets(get_character());

	if (handle) g_factory->destroy_character_handle(std::exchange(handle, nullptr));
	if (blip)	blip.reset();

	if (!is_local() && game_player)
		game_player->set_character(nullptr);
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

		if (!game_player)
		{
			game_player = g_world->get_local();
			game_player->block_key_input(false);
			game_player->block_mouse_input(false);
		}
	}
	else
	{
		// create GamePlayer instance if it wasn't created

		if (!game_player)
			game_player = GamePlayer::CREATE();

		// create and spawn the character if it's not the localplayer

		handle = g_factory->spawn_character("female1");

		check(handle, "Could not create the player's character");

		log(PURPLE, "Player {:x} spawned now {:x} - handle {:x}", get_nid(), ptr(get_character()), ptr(handle));

		blip.reset();
		blip = g_factory->create_map_icon("player_blip", get_position());

		check(blip, "Could not create the player's blip");

		game_player->set_character(get_character());
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
		IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_added_velocity(get_velocity()); });
		break;
	}
	case NetObjectVar_Health:
	{
		IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_hp(get_hp()); });

		// if old health was bigger than 0 but new one is 0
		// it means we died so trigger killed event
		
		if (was_just_killed())
			g_rsrc->trigger_event(script::event::ON_PLAYER_KILLED, this);
		else if (was_just_revived())
		{
#ifdef JC_CLIENT
			// if the player was just revived, make sure
			// we respawn the character

			respawn_character();
#else
#endif
		}

		break;
	}
	case NetObjectVar_MaxHealth:
	{
		IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_max_hp(get_max_hp()); });
		break;
	}
	}
}

void Player::serialize_derived(const Packet* p)
{
	p->add(get_skin_info());
	p->add(get_skin());
	p->add(get_nick());
}

void Player::deserialize_derived(const Packet* p)
{
	set_skin_info(p->get<SkinInfo>());
	set_skin(p->get_i32());
	set_nick(p->get_str());
}

void Player::serialize_derived_create(const Packet* p)
{
}

void Player::deserialize_derived_create(const Packet* p)
{
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

	respawn_character();

	// if this is the localplayer, we will update the 
	// info from the character itself and then set 
	// the net vars so it can call the notify event

	if (is_local())
	{
		character->set_transform(Transform(position, angles));
		character->set_skin(skin, false);
		character->set_hp(hp);
		character->set_max_hp(max_hp);
	}
#else
	Packet p(PlayerPID_Respawn, ChannelID_Generic, this, position, rotation, skin, hp, max_hp);

	g_net->send_broadcast(p);
#endif

	set_transform(TransformTR(position, angles));
	set_skin(skin);
	set_hp(hp, true);
	set_max_hp(max_hp);
}

void Player::set_tag_enabled(bool v)
{
#ifdef JC_CLIENT
	tag_enabled = v;
#else
	// todojc
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

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_skin(v, false); });
}

void Player::set_skin(int32_t v, int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props)
{
	set_skin(v);
	set_skin_info(cloth_skin, head_skin, cloth_color, props);

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_skin(v, cloth_skin, head_skin, cloth_color, props, false); });
}

void Player::set_walking_set_and_skin(int32_t walking_set_id, int32_t skin_id)
{
	dyn_info.walking_set = walking_set_id;
	dyn_info.skin = skin_id;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_walking_anim_set(walking_set_id, skin_id, false); });
}

void Player::set_velocity(const vec3& v)
{
	dyn_info.velocity = v;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_proxy_velocity(v); });
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

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c)
	{
		game_player->set_right(right);
		game_player->set_forward(forward);
	});
}

void Player::crouch(bool enabled)
{
	move_info.is_crouching = enabled;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character*) { game_player->crouch(enabled, false); });
}

void Player::set_state_id(int32_t id)
{
	dyn_info.state_id = id;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character*) { game_player->set_state(id); });
}

void Player::set_body_stance_id(uint32_t id)
{
	dyn_info.body_stance_id = id;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_body_stance(id); });
}

void Player::set_arms_stance_id(uint32_t id)
{
	dyn_info.arms_stance_id = id;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_arms_stance(id); });
}

void Player::set_animation(const std::string& anim_name, float speed, bool unk0, bool unk1)
{
	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_animation(anim_name, speed, unk0, unk1); });
}

void Player::set_head_rotation(const vec3& v, float interpolation)
{
	dyn_info.head_rotation = v;
	dyn_info.head_interpolation = interpolation;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c)
	{
		if (!is_hip_aiming() && !is_full_aiming())
		{
			const auto skeleton = c->get_skeleton();

			skeleton->set_head_euler_rotation(v);
			skeleton->set_head_interpolation(interpolation);
		}
	});
}

void Player::do_punch()
{
	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->setup_punch(); });
}

void Player::force_launch(const vec3& vel, const vec3& dir, float f1, float f2)
{
	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->force_launch(vel, dir, f1, f2); });
}

void Player::set_weapon_id(int32_t id)
{
	if (dyn_info.weapon_id == id)
		return;
	
	dyn_info.weapon_id = id;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_weapon(id); });
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

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->fire_current_weapon(weapon_id, muzzle, dyn_info.aim_target); });
}

void Player::reload()
{
	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->reload_current_weapon(); });
}

void Player::set_skin_info(const SkinInfo& info)
{
	set_skin_info(info.cloth_skin, info.head_skin, info.cloth_color, info.props);
}

void Player::set_skin_info(int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props)
{
	skin_info.cloth_skin = cloth_skin;
	skin_info.head_skin = head_skin;
	skin_info.cloth_color = cloth_color;
	skin_info.props = props;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { c->set_npc_variant(cloth_skin, head_skin, cloth_color, props, false); });
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

void Player::set_in_parachute(bool v)
{
	move_info.paragliding = v;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c)
	{
		if (const auto parachute = game_player->get_parachute())
			if (const bool is_parachute_open = !parachute->is_closed(); !is_parachute_open && v)
			{
				set_body_stance_id(27);
				set_body_stance_id(25);
				set_body_stance_id(59);

				parachute->set_closed(false);
			}
			else if (is_parachute_open && !v)
			{
				if (c->get_body_stance()->get_distance_to_ground() <= StanceController::MAX_DISTANCE_PARACHUTE_FALL() * 2.f)
					set_body_stance_id(27);
				else set_body_stance_id(25);

				if (!parachute->is_closed())
					parachute->set_closed(true);
			}
	});
}

void Player::set_grappled_object(NetObject* obj, const vec3& relative_position)
{
	grappled_object = obj;
	grappled_relative_position = relative_position;

	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c)
	{
		if (obj)
			if (const auto object = obj->get_object_base()->get_shared())
				return c->set_grappled_object(object);

		c->set_grappled_object({});
	});
}

bool Player::is_climbing_ladder()
{
	IF_CLIENT_AND_VALID_CHARACTER_DO([&](Character* c) { move_info.climbing_ladder = c->is_climbing_ladder(); });

	return move_info.climbing_ladder;
}

bool Player::is_crouching()
{
	return move_info.is_crouching;
}

bool Player::is_paragliding()
{
	return move_info.paragliding;
}