#include <defs/standard.h>

#include "logic.h"
#include "net.h"

#include <timer/timer.h>

#include <game/sys/world/world.h>
#include <game/sys/time/time_system.h>

#include <game/object/base/comps/physical.h>
#include <game/object/character/character.h>
#include <game/object/weapon/weapon.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/vehicle/vehicle.h>

#include <core/keycode.h>

#include <dukpp/dukpp.hpp>

static void native_print(dukpp::duk_value const& arg) {
	std::cout << arg << std::endl;
}

class Dog {
	std::string name;
public:
	Dog() : name("") {
		std::cout << "Dog created" << std::endl;
	}

	~Dog() {
		std::cout << "Dog deleted" << std::endl;
	}

	void SetName(std::optional<std::string> const& n) {
		name = n.value_or("Dog");
	}

	std::optional<std::string> GetName() const {
		return name;
	}

	void Bark() const {
		std::cout << "Bark!" << std::endl;
	}
};

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
			const auto vehicle_seat = local_char->get_vehicle_seat();
			const auto& move_info = localplayer->get_movement_info();
			const auto move_angle = move_info.angle;

			// this is some debug we will use when reversing shared and weak ptr

			/*if (const auto seat_ = jc::read<ptr>(local_char, 0x878))
				if (const auto vehicle = jc::read<Vehicle*>(seat_, 0x4C))
				{
					if (g_key->is_key_down(KEY_L))
					{
						static ref<VehicleSeat>* seat = (ref<VehicleSeat>*)malloc(sizeof(ref<VehicleSeat>));

						jc::v_call(vehicle, jc::vehicle::vt::GET_DRIVER_SEAT, seat);

						log(WHITE, "----------------");
						log(GREEN, "1 -> {} {}", seat->counter ? seat->counter->uses : -1, seat->counter ? seat->counter->weaks : -1);

						jc::this_call(0x41F8C0, seat->counter);

						log(GREEN, "2 -> {} {}", seat->counter ? seat->counter->uses : -1, seat->counter ? seat->counter->weaks : -1);

						jc::this_call(0x40E720, &seat->counter->vt);

						log(GREEN, "3 -> {} {}", seat->counter ? seat->counter->uses : -1, seat->counter ? seat->counter->weaks : -1);
					}
				}*/

			// health

			if (hp != localplayer->get_hp())
				localplayer->set_hp(hp);

			if (max_hp != localplayer->get_max_hp())
				localplayer->set_max_hp(max_hp);

			// resync state

			if (state_sync_timer.ready())
			{
				const auto vehicle = vehicle_seat ? vehicle_seat->get_vehicle() : nullptr;
				const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle);

				Packet p(PlayerPID_StateSync, ChannelID_Generic, current_weapon_id, vehicle_net);

				if (vehicle_net)
					p.add(vehicle_seat->get_type());

				g_net->send(p);
			}

			// we need to check if we are in a vehicle or not, if so, we won't send stuff such as our transform,
			// the movement info etc we will save a lot of bandwidth and performance with this optimization

			if (!vehicle_seat)
			{
				// movement info & movement angle too

				if (localplayer->should_force_sync_movement_info())
				{
					const auto packed_angle = util::pack::pack_pi_angle(move_info.angle);
					const auto packed_right = util::pack::pack_norm(move_info.right);
					const auto packed_forward = util::pack::pack_norm(move_info.forward);

					g_net->send(Packet(PlayerPID_StanceAndMovement, ChannelID_Generic, PlayerStanceID_Movement, packed_angle, packed_right, packed_forward, move_info.aiming));
				}
				else if (localplayer->should_sync_angle_only() && angle_timer.ready())
					g_net->send(Packet(PlayerPID_StanceAndMovement, ChannelID_Generic, PlayerStanceID_MovementAngle, util::pack::pack_pi_angle(move_angle)));


				localplayer->set_movement_angle(move_angle, false);

				// transform (we upload it every 100 ms for now to correct the position in remote players)

				const bool is_on_ground = local_char->is_on_ground();

				if ((position != localplayer->get_position() || rotation != localplayer->get_rotation()) &&
					(is_on_ground && transform_timer.ready()) || (!is_on_ground && fast_transform_timer.ready()))
				{
					TransformTR transform_tr(position, rotation);

					g_net->send(Packet(WorldPID_SyncObject, ChannelID_World, localplayer, NetObjectVar_Transform, transform_tr.pack()).set_unreliable());

					localplayer->set_transform(transform_tr);
				}
			}
			else if (const auto vehicle = vehicle_seat->get_vehicle())
			{
				const auto vehicle_position = vehicle->get_position();
				const auto vehicle_rotation = vehicle->get_rotation();

				TransformTR vehicle_transform(vehicle_position, vehicle_rotation);

				localplayer->set_transform(vehicle_transform);
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
				g_net->send(Packet(PlayerPID_DynamicInfo, ChannelID_Generic, PlayerDynInfo_HeadRotation, head_rotation, util::pack::pack_norm(head_interpolation)).set_unreliable());

				localplayer->set_head_rotation(head_rotation, head_interpolation);
			}

			// current weapon id switching

			if (current_weapon_id != localplayer->get_weapon_id())
			{
				g_net->send(Packet(PlayerPID_SetWeapon, ChannelID_Generic, current_weapon_id));

				localplayer->set_weapon_id(current_weapon_id);
			}

			// aiming

			if ((hip_aiming || full_aiming) && aiming_timer.ready())
				g_net->send(Packet(PlayerPID_StanceAndMovement, ChannelID_Generic, PlayerStanceID_Aiming, hip_aiming, full_aiming, aim_target).set_unreliable());

			// debug

			if (g_key->is_key_pressed(VK_F3))
			{
				TransformTR transform(position + vec3(2.f, 1.f, 0.f));

				g_net->send(Packet(WorldPID_SpawnObject, ChannelID_World, NetObject_Damageable, 0ui16, transform));
			}

			if (g_key->is_key_pressed(VK_F4))
			{
				TransformTR transform(position + vec3(2.f, 1.f, 0.f));

				g_net->send(Packet(WorldPID_SpawnObject, ChannelID_World, NetObject_Vehicle, 51ui16, transform));
			}

			if (g_key->is_key_pressed(VK_NUMPAD9))
			{
				dukpp::context ctx;

				ctx["print"] = native_print;
				ctx["AY"] = 5.1238f;

				ctx["trash"] = +[](const dukpp::variadic_args& args)
				{
					float res = 0.f;

					for (auto const& val : args)
						res += val.as_float();

					return res;
				};

				ctx.register_class<Dog>("Dog")
					.add_method("bark", &Dog::Bark)
					.add_property("name", &Dog::GetName, &Dog::SetName);

				ctx.peval(R"(
function tick()
{
	return trash(1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5, 11.5, 12.5, 13.5, 14.5, 15.5);
}
)");

				//log(RED, "exists 1: {}", duk_get_global_string(ctx.mCtx, "ticka"));
				//log(RED, "exists 2: {}", duk_is_function(ctx.mCtx, -1));

				std::string tick_fn = "tick";

				while (true)
				{
					float res = 0.f;

					{
						TimeProfiling p("test");

						try
						{
							res = ctx.call<float>(tick_fn);
						}
						catch (...)
						{
							//log(RED, "sad error");
						}
					}

					log(GREEN, "result: {}", res);

					Sleep(250);
				}
			}
		}
}

void jc::mp::logic::on_update_objects()
{
	g_net->for_each_net_object([](NID, NetObject* obj)
	{
		// it's possible that this object is far away from the localplayer
		// so it won't be updated since the game object itself does not exist

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
			const auto seat_type = player->get_vehicle_seat();
			const auto char_vehicle = player_char->get_vehicle();

			// update player's blip

			player->update_blip();

			// correct player position with the server's transform

			player->correct_position();

			// update the player movement

			player->dispatch_movement();

			// make sure we put the player inside or outside the vehicle

			if (vehicle && char_vehicle != vehicle)
				vehicle_net->warp_to_seat(player, seat_type);
			else if (!vehicle && char_vehicle)
			{
				if (const auto seat = char_vehicle->get_seat_by_type(seat_type))
					seat->instant_exit();
			}

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