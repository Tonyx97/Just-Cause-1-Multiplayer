#include <defs/standard.h>

#include "logic.h"
#include "net.h"

#include <timer/timer.h>

#include <game/sys/world.h>
#include <game/object/character/character.h>
#include <game/object/weapon/weapon.h>
#include <game/object/weapon/weapon_belt.h>

#include <game/sys/time_system.h>

#include <core/keycode.h>

void jc::mp::logic::on_tick()
{
	// send and update our local player info

	static TimerRaw state_sync_timer(2500);
	static TimerRaw transform_timer(100);
	static TimerRaw velocity_timer(3000);
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
			const auto current_weapon_id = current_weapon_info ? current_weapon_info->get_id() : 0;
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
			const auto& move_info = localplayer->get_movement_info();
			const auto move_angle = move_info.angle;

			// health

			if (hp != localplayer->get_hp())
				localplayer->set_hp(hp);

			if (max_hp != localplayer->get_max_hp())
				localplayer->set_max_hp(max_hp);

			// resync state

			if (state_sync_timer.ready())
				g_net->send_reliable(PlayerPID_StateSync, current_weapon_id);

			// transform (we upload it every 500 ms for now to correct the position in remote players)

			if ((position != localplayer->get_position() || rotation != localplayer->get_rotation()) && local_char->is_on_ground() && transform_timer.ready())
			{
				TransformTR transform_tr(position, rotation);

				g_net->send_unreliable<ChannelID_World>(WorldPID_SyncObject, localplayer, NetObjectVar_Transform, transform_tr.pack());

				localplayer->set_transform(transform_tr);
			}

			// velocity
			
			/*if (velocity != localplayer->get_velocity() && glm::length(velocity) > 10.f && velocity_timer.ready())
			{
				g_net->send_unreliable<ChannelID_World>(WorldPID_SyncObject, NetObjectVar_Velocity, velocity);

				localplayer->set_velocity(velocity);
			}*/

			// movement angle

			if (localplayer->should_sync_angle_only() && angle_timer.ready())
			{
				g_net->send_unreliable(PlayerPID_StanceAndMovement, PlayerStanceID_MovementAngle, util::pack::pack_pi_angle(move_angle));

				localplayer->set_movement_angle(move_angle, false);
			}

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

			if (g_key->is_key_pressed(VK_F3))
			{
				TransformTR transform(position + vec3(2.f, 1.f, 0.f));

				g_net->send_reliable<ChannelID_World>(WorldPID_SpawnObject, NetObject_Damageable, transform);
			}
		}
}

void jc::mp::logic::on_update_objects()
{
	g_net->for_each_net_object([](NID, NetObject* obj)
	{
		if (!obj->is_spawned())
			return;

		switch (const auto type = obj->get_type())
		{
		case NetObject_Player:
		{
			const auto player = obj->cast<Player>();

			if (player->is_local())
				return;

			const auto player_char = player->get_character();

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
				auto previous_aim_target = player_char->get_aim_target(),
					 target_aim_target = player->get_aim_target();

				player_char->set_aim_target(glm::lerp(previous_aim_target, target_aim_target, 0.1f));
			}

			break;
		}
		case NetObject_Damageable:
		{
			const auto damageable = obj->cast<DamageableNetObject>();

			// check if we own this damageable, if so,
			// let's sync it with the server and other players

			if (!damageable->sync())
			{
				// if this object is not owned then we will simply update the needed stuff
			}

			break;
		}
		default: log(RED, "Unknown net object type {}", type);
		}
	});
}