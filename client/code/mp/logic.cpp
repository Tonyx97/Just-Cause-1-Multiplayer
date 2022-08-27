#include <defs/standard.h>

#include "logic.h"
#include "net.h"

#include <timer/timer.h>

#include <game/sys/world/world.h>
#include <game/sys/time/time_system.h>

#include <game/object/base/comps/physical.h>
#include <game/object/character/character.h>
#include <game/object/character/comps/vehicle_controller.h>
#include <game/object/character/comps/vehicle_controller.h>
#include <game/object/weapon/weapon.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/vehicle/vehicle.h>

#include <core/keycode.h>

void jc::mp::logic::on_tick()
{
	if (!g_net->is_joined())
		return;

	// send and update our local player info

	static TimerRaw state_sync_timer(2500);
	static TimerRaw transform_timer(enet::TICKS_MS * 5);
	static TimerRaw fast_transform_timer(enet::TICKS_MS);
	static TimerRaw velocity_timer(enet::TICKS_MS * 5);
	static TimerRaw angle_timer(16 * 2);
	static TimerRaw head_rotation_timer(enet::TICKS_MS * 25);
	static TimerRaw aiming_timer(enet::TICKS_MS * 5);

	if (auto localplayer = g_net->get_localplayer())
		if (const auto local_char = localplayer->get_character())
		{
			const auto skeleton = local_char->get_skeleton();
			const auto weapon_belt = local_char->get_weapon_belt();
			const auto current_weapon = weapon_belt->get_current_weapon();
			const auto current_weapon_info = current_weapon ? current_weapon->get_info() : nullptr;
			const auto current_weapon_id = current_weapon_info ? current_weapon_info->get_id() : 0ui8;
			const auto flags = local_char->get_flags();
			const auto transform = local_char->get_transform();
			const auto velocity = local_char->get_velocity();
			const auto position = transform.get_position();
			const auto rotation = transform.get_rotation();
			const auto hp = local_char->get_real_hp();
			const auto max_hp = local_char->get_max_hp();
			const auto head_rotation = skeleton->get_head_euler_rotation();
			const auto head_interpolation = skeleton->get_head_interpolation();
			const bool hip_aiming = localplayer->is_hip_aiming();
			const bool full_aiming = localplayer->is_full_aiming();
			const auto aim_target = local_char->get_aim_target();
			const auto vehicle = local_char->get_vehicle();
			const auto& move_info = localplayer->get_movement_info();
			const auto move_angle = move_info.angle;

			/*const auto vehicle_net = localplayer->get_vehicle();
			const auto vehicle = vehicle_net ? vehicle_net->get_object() : nullptr;
			const auto old_vehicle = local_char->get_vehicle();*/

			// health

			if (hp != localplayer->get_hp())
				localplayer->set_hp(hp);

			if (max_hp != localplayer->get_max_hp())
				localplayer->set_max_hp(max_hp);

			// resync state

			if (state_sync_timer.ready())
				g_net->send_reliable(PlayerPID_StateSync, current_weapon_id, g_net->get_net_object_by_game_object(vehicle));

			// we need to check if we are in a vehicle or not, if so, we won't send stuff such as our transform,
			// the movement info etc we will save a lot of bandwidth and performance with this optimization

			if (!vehicle)
			{
				// movement info & movement angle too

				if (localplayer->should_force_sync_movement_info())
				{
					const auto packed_angle = util::pack::pack_pi_angle(move_info.angle);
					const auto packed_right = util::pack::pack_norm(move_info.right);
					const auto packed_forward = util::pack::pack_norm(move_info.forward);

					g_net->send_reliable(PlayerPID_StanceAndMovement, PlayerStanceID_Movement, packed_angle, packed_right, packed_forward, move_info.aiming);
				}
				else if (localplayer->should_sync_angle_only() && angle_timer.ready())
					g_net->send_reliable(PlayerPID_StanceAndMovement, PlayerStanceID_MovementAngle, util::pack::pack_pi_angle(move_angle));


				localplayer->set_movement_angle(move_angle, false);

				// transform (we upload it every 100 ms for now to correct the position in remote players)

				const bool is_on_ground = local_char->is_on_ground();

				if ((position != localplayer->get_position() || rotation != localplayer->get_rotation()) &&
					(is_on_ground && transform_timer.ready()) || (!is_on_ground && fast_transform_timer.ready()))
				{
					TransformTR transform_tr(position, rotation);

					g_net->send_unreliable<ChannelID_World>(WorldPID_SyncObject, localplayer, NetObjectVar_Transform, transform_tr.pack());

					localplayer->set_transform(transform_tr);
				}
			}

			// velocity
			
			/*if (velocity != localplayer->get_velocity() && glm::length(velocity) > 0.f && velocity_timer.ready())
			{
				g_net->send_unreliable<ChannelID_World>(WorldPID_SyncObject, NetObjectVar_Velocity, velocity);

				localplayer->set_velocity(velocity);

				log(GREEN, "sending vel {}", glm::length(velocity));
			}*/

			// head rotation
			
			if (!hip_aiming &&
				!full_aiming &&
				head_interpolation == 1.f &&
				(glm::distance(head_rotation, localplayer->get_head_rotation()) > 5.f ||
				head_interpolation != localplayer->get_head_interpolation()) &&
				head_rotation_timer.ready())
			{
				g_net->send_unreliable(PlayerPID_DynamicInfo, PlayerDynInfo_HeadRotation, head_rotation, util::pack::pack_norm(head_interpolation));

				localplayer->set_head_rotation(head_rotation, head_interpolation);
			}

			// current weapon id switching

			if (current_weapon_id != localplayer->get_weapon_id())
			{
				g_net->send_reliable(PlayerPID_SetWeapon, current_weapon_id);

				localplayer->set_weapon_id(current_weapon_id);
			}

			// aiming

			if ((hip_aiming || full_aiming) && aiming_timer.ready())
				g_net->send_unreliable(PlayerPID_StanceAndMovement, PlayerStanceID_Aiming, hip_aiming, full_aiming, aim_target);

			// debug

			/*if (g_key->is_key_pressed(VK_F3))
			{
				TransformTR transform(position + vec3(2.f, 1.f, 0.f));

				g_net->send_reliable<ChannelID_World>(WorldPID_SpawnObject, NetObject_Damageable, 0ui16, transform);
			}*/

			if (g_key->is_key_pressed(VK_F4))
			{
				TransformTR transform(position + vec3(2.f, 1.f, 0.f));

				g_net->send_reliable<ChannelID_World>(WorldPID_SpawnObject, NetObject_Vehicle, 51ui16, transform);
			}
		}
}

void jc::mp::logic::on_update_objects()
{
	g_net->for_each_net_object([](NID, NetObject* obj)
	{
		if (!obj->is_spawned())
			return;

		const auto obj_base = obj->get_object_base();

		switch (const auto type = obj->get_type())
		{
		case NetObject_Player:
		{
			const auto player = obj->cast<Player>();

			if (player->is_local())
				return;

			const auto player_char = player->get_character();
			const auto vehicle_net = player->get_vehicle();
			const auto vehicle = vehicle_net ? vehicle_net->get_object() : nullptr;
			const auto old_vehicle = player_char->get_vehicle();

			// update player's blip

			player->update_blip();

			// correct player position with the server's transform

			player->correct_position();

			// update the player movement

			player->dispatch_movement();

			// aiming weapon

			bool hip_aim = false,
				 full_aim = false;

			if (hip_aim = player->is_hip_aiming())
				player_char->set_flag(CharacterFlag_HipAiming);
			else player_char->remove_flag(CharacterFlag_HipAiming);

			if (full_aim = player->is_full_aiming())
				player_char->set_flag(CharacterFlag_FullAiming);
			else player_char->remove_flag(CharacterFlag_FullAiming);

			if (hip_aim || full_aim)
			{
				const auto previous_aim_target = player_char->get_aim_target(),
						   target_aim_target = player->get_aim_target();

				player_char->set_aim_target(jc::math::lerp(previous_aim_target, target_aim_target, 0.075f));
			}

			break;
		}
		case NetObject_Damageable:
		{
			// check if we own this damageable, if so,
			// let's sync it with the server and other players

			if (!obj->sync())
			{
				// if this object is not owned then we will simply update the needed stuff

				const auto pos = obj->get_position();
				const auto rot = obj->get_rotation();

				obj_base->set_transform(obj_base->get_transform().interpolate(Transform(pos, rot), 0.2f, 0.2f));
			}

			break;
		}
		case NetObject_Blip:
		{
			if (!obj->sync())
			{
				const auto pos = obj->get_position();
				const auto rot = obj->get_rotation();

				obj_base->set_transform(obj_base->get_transform().interpolate(Transform(pos, rot), 0.2f, 0.2f));
			}

			break;
		}
		case NetObject_Vehicle:
		{
			if (!obj->sync())
			{
			}

			break;
		}
		default: log(RED, "Unknown net object type {}", type);
		}
	});
}