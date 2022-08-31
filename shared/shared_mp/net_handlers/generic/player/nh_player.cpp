#include <defs/standard.h>

#include "nh_player.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

// debug

#ifdef JC_CLIENT
#include <game/object/character/character.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/base/comps/physical.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/vehicle/comps/vehicle_seat.h>
#include <game/object/interactable/interactable.h>
#endif

enet::PacketResult nh::player::state_sync(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto curr_weapon = p.get_u8();
	const auto curr_vehicle = p.get_net_object();

	player->set_weapon_id(curr_weapon);
	//player->set_vehicle(curr_vehicle->cast<VehicleNetObject>());

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_StateSync, player, curr_weapon, curr_vehicle);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::respawn(const enet::Packet& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
	{
		const auto hp = p.get_float(),
				   max_hp = p.get_float();

		player->respawn(hp, max_hp, false);
	}
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();

	const auto hp = p.get_float(),
			   max_hp = p.get_float();

	player->respawn(hp, max_hp);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::dynamic_info(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	// dispatch all dynamic infos sent in this packet

	while (!p.is_empty())
	{
		const auto type = p.get_u8();

		switch (type)
		{
		
		case PlayerDynInfo_HeadRotation:
		{
			const auto rotation = p.get_raw<vec3>();
			const auto interpolation = p.get_i8();

			player->set_head_rotation(rotation, util::pack::unpack_norm(interpolation));

#ifdef JC_SERVER
			g_net->send_broadcast_unreliable(pc, PlayerPID_DynamicInfo, player, type, rotation, interpolation);
#endif

			break;
		}
		case PlayerDynInfo_Skin:
		{
			const auto skin_id = p.get_i32();

			player->set_skin(skin_id);

#ifdef JC_SERVER
			g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, skin_id);
#endif

			break;
		}
		case PlayerDynInfo_WalkingSetAndSkin:
		{
			const auto walking_anim_set_id = p.get_i32();
			const auto skin_id = p.get_i32();

			player->set_walking_set_and_skin(walking_anim_set_id, skin_id);

#ifdef JC_SERVER
			g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, walking_anim_set_id, skin_id);
#endif

			break;
		}
		case PlayerDynInfo_NPCVariant:
		{
			const auto cloth_skin = p.get_i32(),
					   head_skin = p.get_i32(),
					   cloth_color = p.get_i32();

			auto accessories = p.get_vector<VariantPropInfo>();

			player->set_skin_info(cloth_skin, head_skin, cloth_color, accessories);

#ifdef JC_SERVER
			g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, cloth_skin, head_skin, cloth_color, std::move(accessories));
#endif

			break;
		}
		}
	}

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::stance_and_movement(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();
	
	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto type = p.get_u8();

	switch (type)
	{
	case PlayerStanceID_Movement:
	{
		const auto angle = p.get_i16();
		const auto right = p.get_i8();
		const auto forward = p.get_i8();
		const auto aiming = p.get_bool();

		player->set_movement_info(util::pack::unpack_pi_angle(angle), util::pack::unpack_norm(right), util::pack::unpack_norm(forward), aiming);

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, angle, right, forward, aiming);
#endif

		break;
	}
	case PlayerStanceID_MovementAngle:
	{
		const auto angle = p.get_i16();

		player->set_movement_angle(util::pack::unpack_pi_angle(angle), false);

#ifdef JC_SERVER
		g_net->send_broadcast_unreliable(pc, PlayerPID_StanceAndMovement, player, type, angle);
#endif

		break;
	}
	case PlayerStanceID_Jump:
	{
		player->set_body_stance_id(BodyStance_Jump);

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type);
#endif

		break;
	}
	case PlayerStanceID_Punch:
	{
		player->do_punch();

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type);
#endif

		break;
	}
	case PlayerStanceID_BodyStance:
	{
		const auto stance_id = p.get_u32();

		player->set_body_stance_id(stance_id);

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, stance_id);
#endif

		break;
	}
	case PlayerStanceID_Aiming:
	{
		const auto hip_aiming = p.get_bool(),
				   full_aiming = p.get_bool();

		const auto target_pos = p.get_raw<vec3>();

		player->set_aim_info(hip_aiming, full_aiming, target_pos);

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, hip_aiming, full_aiming, target_pos);
#endif

		break;
	}
	case PlayerStanceID_Fire:
	{
		const auto weapon_id = p.get_u8();
		const auto muzzle_pos = p.get_raw<vec3>();
		const auto packed_direction = p.get_raw<i16vec3>();

#ifdef JC_CLIENT
		player->set_multiple_rand_seed(0u);
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, weapon_id, muzzle_pos, packed_direction);
#endif

		player->fire_current_weapon(weapon_id, muzzle_pos, jc::math::unpack_vec3(packed_direction, 1.f));

		break;
	}
	case PlayerStanceID_FireMultiple:
	{
		const auto rand_seed = p.get_u16();
		const auto weapon_id = p.get_u8();
		const auto muzzle_pos = p.get_raw<vec3>();
		const auto packed_direction = p.get_raw<i16vec3>();

#ifdef JC_CLIENT
		player->set_multiple_rand_seed(rand_seed);
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, rand_seed, weapon_id, muzzle_pos, packed_direction);
#endif

		player->fire_current_weapon(weapon_id, muzzle_pos, jc::math::unpack_vec3(packed_direction, 1.f));

		break;
	}
	case PlayerStanceID_Reload:
	{
		player->reload();

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type);
#endif

		break;
	}
	case PlayerStanceID_ForceLaunch:
	{
		const auto vel = p.get_raw<vec3>();
		const auto dir = p.get_raw<vec3>();
		const auto f1 = p.get_float();
		const auto f2 = p.get_float();

		player->force_launch(vel, dir, f1, f2);

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, vel, dir, f1, f2);
#endif

		break;
	}
	}

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::set_weapon(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto weapon_id = p.get_u8();

	player->set_weapon_id(weapon_id);

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_SetWeapon, player, weapon_id);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::set_vehicle(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
	const auto curr_vehicle = player->get_vehicle();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();
	const auto seat_type = p.get_u8();

	//log(GREEN, "VehicleEnterExit_SetVehicle {:x}", ptr(vehicle_net));

#ifdef JC_SERVER
	// if the seat type is the driver then we want to set the sync type and
	// the streamer of this vehicle

	if (seat_type == VehicleSeat_Driver)
	{
		if (vehicle_net)
		{
			// if the target seat is driver and the vehicle is valid
			// then make this player the streamer

			log(BLUE, "LOCKED DRIVER {:x}", player->get_nid());

			vehicle_net->set_sync_type_and_streamer(SyncType_Locked, player);
		}
		else if (curr_vehicle)
		{
			// if the vehicle wasn't valid but the player was in a vehicle
			// then change the streaming info

			const auto driver = curr_vehicle->get_player_from_seat(VehicleSeat_Driver);

			if (player == driver)
			{
				// the vehicle is empty so just set it to distanced and
				// reset the streamer

				log(BLUE, "DISTANCED {:x}", player->get_nid());

				curr_vehicle->set_sync_type_and_streamer(SyncType_Distance, nullptr);
			}
		}
	}

	g_net->send_broadcast_reliable(pc, PlayerPID_SetVehicle, player, vehicle_net, seat_type);
#endif

	player->set_vehicle(seat_type, vehicle_net);

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::enter_exit_vehicle(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
	const auto curr_vehicle = player->get_vehicle();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();
	const auto seat_type = p.get_u8();
	const auto command = p.get_u8();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

#ifdef JC_CLIENT
	const auto vehicle = vehicle_net->get_object();
	const auto player_char = player->get_character();
	const auto seat = vehicle->get_seat_by_type(seat_type);

	if (!vehicle || !player_char || !seat)
		return enet::PacketRes_BadArgs;
#endif

	log(GREEN, "vehicle seat type: {}", seat_type);

	switch (command)
	{
	case VehicleEnterExit_RequestEnter:
	{
		//log(GREEN, "VehicleEnterExit_RequestEnter");

#ifdef JC_CLIENT
		const auto interactable = seat->get_interactable();

		interactable->interact_with(player_char);
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_EnterExitVehicle, player, vehicle_net, seat_type, command);
#endif

		break;
	}
	case VehicleEnterExit_Exit:
	{
		const bool jump_out = p.get_bool();

		//log(GREEN, "VehicleEnterExit_Exit (jump out: {})", jump_out);

#ifdef JC_CLIENT
		if (seat->is_instant_exit())
			seat->instant_exit();
		else seat->exit(jump_out);
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_EnterExitVehicle, player, vehicle_net, seat_type, command, jump_out);
#endif

		break;
	}
	case VehicleEnterExit_PassengerToDriver:
	{
		const auto victim = p.get_net_object<Player>();

		//log(GREEN, "VehicleEnterExit_PassengerToDriver {:x}... ", victim ? victim->get_nid() : INVALID_NID);

#ifdef JC_CLIENT
		const auto passenger_seat = vehicle->get_passenger_seat();

		// make the passenger move to the driver seat

		passenger_seat->add_flag(VehicleSeatFlag_PassengerToDriverSeat);

		// if there is a driver then kick him out

		if (const auto victim_char = victim ? victim->get_character() : nullptr)
		{
			victim_char->set_stance_exit_vehicle_forced();
			vehicle->open_door(VehicleDoor_Left);
		}
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_EnterExitVehicle, player, vehicle_net, seat_type, command, victim);
#endif

		break;
	}
	case VehicleEnterExit_DriverToRoof:
	{
		//log(GREEN, "VehicleEnterExit_DriverToRoof...");

#ifdef JC_CLIENT
		/*const auto driver_seat = vehicle->get_driver_seat();

		// make the driver move to the roof seat
		// todojc - the character won't get attached to the roof :(

		(*(void(__thiscall**)(Vehicle*, int))(*(ptr*)vehicle + 0xE8))(vehicle, 1);
		vehicle->detach_door(VehicleDoor_Left);
		driver_seat->add_flag2(VehicleSeatFlag_DriverToRoofSeat);
		driver_seat->set_timer(1.f);
		driver_seat->add_flag2(1 << 6);*/
#else
		//g_net->send_broadcast_reliable(pc, PlayerPID_EnterExitVehicle, player, vehicle_net, seat_type, command);
#endif

		break;
	}
	}

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::vehicle_control(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

#ifdef JC_SERVER
	// check if we should update and broadcast the new info

	if (!vehicle_net->is_owned_by(player))
		return enet::PacketRes_NotAllowed;
#endif

	const auto packed_info = p.get_raw<VehicleNetObject::PackedControlInfo>();
	const auto unpacked_info = VehicleNetObject::ControlInfo(packed_info);

	vehicle_net->set_control_info(unpacked_info);

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleControl, player, vehicle_net, packed_info);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::vehicle_honk(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

#ifdef JC_CLIENT
	const auto vehicle = vehicle_net->get_object();

	vehicle->honk();
#endif

#ifdef JC_SERVER
	if (!vehicle_net->is_owned_by(player))
		return enet::PacketRes_NotAllowed;

	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleHonk, vehicle_net);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::vehicle_engine_state(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

#ifdef JC_SERVER
	if (!vehicle_net->is_owned_by(player))
		return enet::PacketRes_NotAllowed;
#endif

	const bool state = p.get_bool();

#ifdef JC_CLIENT
	const auto vehicle = vehicle_net->get_object();

	vehicle->set_engine_state(state);
#endif

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleEngineState, vehicle_net, state);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::vehicle_fire(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

#ifdef JC_SERVER
	if (!vehicle_net->is_owned_by(player))
		return enet::PacketRes_NotAllowed;
#endif

	const auto weapon_index = p.get_u8();
	const auto weapon_type = p.get_u8();
	const auto fire_info = p.get_vector<VehicleNetObject::FireInfo>();

	vehicle_net->set_weapon_info(weapon_index, weapon_type);
	vehicle_net->set_fire_info(fire_info);

#ifdef JC_CLIENT
	vehicle_net->fire();
#endif

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleFire, vehicle_net, weapon_index, weapon_type, fire_info);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::vehicle_mounted_gun_fire(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

	const auto fire_info = p.get_raw<VehicleNetObject::FireInfoBase>();

	vehicle_net->set_mounted_gun_fire_info(fire_info);

#ifdef JC_CLIENT
	vehicle_net->fire_mounted_gun();
#endif

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleMountedGunFire, vehicle_net, fire_info);
#endif

	return enet::PacketRes_Ok;
}