#include <defs/standard.h>

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

#include "nh_player.h"

// debug

#ifdef JC_CLIENT
#include <game/object/game_player/game_player.h>
#include <game/object/parachute/parachute.h>
#include <game/object/character/character.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/vehicle/comps/vehicle_seat.h>
#include <game/object/interactable/interactable.h>
#endif

PacketResult nh::player::state_sync(const Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto curr_weapon = p.get_u8();
	const auto curr_state = p.get_i32();
	const auto curr_vehicle = p.get_net_object()->cast<VehicleNetObject>();
	const auto seat_type = curr_vehicle ? p.get_u8() : VehicleSeat_None;

#ifdef JC_SERVER
	p.add_beginning(player);

	g_net->send_broadcast(pc, p);
#endif

	player->set_weapon_id(curr_weapon);
	player->set_state_id(curr_state);
	player->set_vehicle(seat_type, curr_vehicle);

	return PacketRes_Ok;
}

PacketResult nh::player::respawn(const Packet& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
	{
		const auto position = p.get<vec3>();
		const auto rotation = p.get_float();
		const auto skin = p.get_i32();
		const auto hp = p.get_float();
		const auto max_hp = p.get_float();

		player->respawn(position, rotation, skin, hp, max_hp);
	}
	else return PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();

	const auto position = p.get<vec3>();
	const auto rotation = p.get_float();
	const auto skin = p.get_i32();
	const auto hp = p.get_float();
	const auto max_hp = p.get_float();

	player->respawn(position, rotation, skin, hp, max_hp);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player::dynamic_info(const Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto type = p.get_u8();

	switch (type)
	{
	case PlayerDynInfo_HeadRotation:
	{
		const auto rotation = p.get<vec3>();
		const auto interpolation = p.get_i8();

		player->set_head_rotation(rotation, util::pack::unpack_norm(interpolation));

		break;
	}
	case PlayerDynInfo_Skin:
	{
		const auto skin_id = p.get_i32();

		player->set_skin(skin_id);

		break;
	}
	case PlayerDynInfo_WalkingSetAndSkin:
	{
		const auto walking_anim_set_id = p.get_i32();
		const auto skin_id = p.get_i32();

		player->set_walking_set_and_skin(walking_anim_set_id, skin_id);

		break;
	}
	case PlayerDynInfo_NPCVariant:
	{
		const auto cloth_skin = p.get_i32(),
					head_skin = p.get_i32(),
					cloth_color = p.get_i32();

		auto accessories = p.get<std::vector<VariantPropInfo>>();

		player->set_skin_info(cloth_skin, head_skin, cloth_color, accessories);

		break;
	}
	}

#ifdef JC_SERVER
	p.add_beginning(player);

	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player::stance_and_movement(const Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto type = p.get_u8();

	switch (type)
	{
	case PlayerStanceID_BodyStance:
	{
		const auto stance_id = p.get_u32();

		player->set_body_stance_id(stance_id);

		break;
	}
	case PlayerStanceID_PlayerMoveState:
	{
		const auto state_id = p.get_i32();

		player->set_state_id(state_id);

		break;
	}
	case PlayerStanceID_Crouch:
	{
		const auto enabled = p.get_bool();

		player->crouch(enabled);

		break;
	}
	case PlayerStanceID_Movement:
	{
		const auto angle = p.get_i16();
		const auto right = p.get_i8();
		const auto forward = p.get_i8();
		const auto aiming = p.get_bool();

		player->set_movement_info(util::pack::unpack_pi_angle(angle), util::pack::unpack_norm(right), util::pack::unpack_norm(forward), aiming);

		break;
	}
	case PlayerStanceID_MovementAngle:
	{
		const auto angle = p.get_i16();

		player->set_movement_angle(util::pack::unpack_pi_angle(angle), false);

		break;
	}
	case PlayerStanceID_Punch:
	{
		player->do_punch();

		break;
	}
	case PlayerStanceID_Aiming:
	{
		const auto hip_aiming = p.get_bool(),
				   full_aiming = p.get_bool();

		const auto target_pos = p.get<vec3>();

		player->set_aim_info(hip_aiming, full_aiming, target_pos);

		break;
	}
	case PlayerStanceID_Fire:
	{
		const auto weapon_id = p.get_u8();
		const auto muzzle_pos = p.get<vec3>();
		const auto packed_direction = p.get<i16vec3>();

#ifdef JC_CLIENT
		player->set_multiple_rand_seed(0u);
#endif

		player->fire_current_weapon(weapon_id, muzzle_pos, jc::math::unpack_vec3(packed_direction, 1.f));

		break;
	}
	case PlayerStanceID_FireMultiple:
	{
		const auto rand_seed = p.get_u16();
		const auto weapon_id = p.get_u8();
		const auto muzzle_pos = p.get<vec3>();
		const auto packed_direction = p.get<i16vec3>();

#ifdef JC_CLIENT
		player->set_multiple_rand_seed(rand_seed);
#endif

		player->fire_current_weapon(weapon_id, muzzle_pos, jc::math::unpack_vec3(packed_direction, 1.f));

		break;
	}
	case PlayerStanceID_Reload:
	{
		player->reload();

		break;
	}
	case PlayerStanceID_ForceLaunch:
	{
		const auto vel = p.get<vec3>();
		const auto dir = p.get<vec3>();
		const auto f1 = p.get_float();
		const auto f2 = p.get_float();

		player->force_launch(vel, dir, f1, f2);

		break;
	}
	}

#ifdef JC_SERVER
	p.add_beginning(player);

	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player::parachute_control(const Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

#ifdef JC_CLIENT
	switch (const auto control_type = p.get_u8())
	{
	case 0: player->set_in_parachute(false); break;
	case 1: player->set_in_parachute(true); break;
	}
#endif

#ifdef JC_SERVER
	p.add_beginning(player);

	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player::grappling_hook(const Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	if (const bool attach = p.get_bool())
	{
		const auto attach_to_net_obj = p.get_net_object<NetObject>();
		const auto relative_attach_pos = p.get<vec3>();

		if (!attach_to_net_obj)
			return PacketRes_BadArgs;

		player->set_grappled_object(attach_to_net_obj, relative_attach_pos);
	}
	else player->set_grappled_object(nullptr);
	
#ifdef JC_SERVER
	p.add_beginning(player);

	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player::set_weapon(const Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto weapon_id = p.get_u8();

	player->set_weapon_id(weapon_id);

#ifdef JC_SERVER
	p.add_beginning(player);

	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player::set_vehicle(const Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return PacketRes_BadArgs;
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

			vehicle_net->set_sync_type_and_owner(SyncType_Locked, player);
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

				curr_vehicle->set_sync_type_and_owner(SyncType_Distance, nullptr);
			}
		}
	}

	p.add_beginning(player);

	g_net->send_broadcast(pc, p);
#endif

	player->set_vehicle(seat_type, vehicle_net);

	return PacketRes_Ok;
}

PacketResult nh::player::enter_exit_vehicle(const Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
	const auto curr_vehicle = player->get_vehicle();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();
	const auto seat_type = p.get_u8();
	const auto command = p.get_u8();

	if (!vehicle_net)
		return PacketRes_BadArgs;

#ifdef JC_CLIENT
	const auto vehicle = vehicle_net->get_object();
	const auto player_char = player->get_character();
	const auto seat = vehicle->get_seat_by_type(seat_type);

	if (!vehicle || !player_char || !seat)
		return PacketRes_BadArgs;
#endif

	//log(GREEN, "vehicle seat type: {}", seat_type);

	switch (command)
	{
	case VehicleEnterExit_RequestEnter:
	{
		//log(GREEN, "VehicleEnterExit_RequestEnter");

#ifdef JC_CLIENT
		const auto interactable = seat->get_interactable();

		interactable->interact_with(player_char);
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
#endif

		break;
	}
	}

#ifdef JC_SERVER
	p.add_beginning(player);

	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}