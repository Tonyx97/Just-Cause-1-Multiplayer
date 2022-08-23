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

	player->set_weapon_id(curr_weapon);

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_StateSync, player, curr_weapon);
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
		const auto direction = p.get_raw<vec3>();

#ifdef JC_CLIENT
		player->set_multiple_rand_seed(0u);
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, weapon_id, muzzle_pos, direction);
#endif

		player->fire_current_weapon(weapon_id, muzzle_pos, direction);

		break;
	}
	case PlayerStanceID_FireMultiple:
	{
		const auto rand_seed = p.get_u16();
		const auto weapon_id = p.get_u8();
		const auto muzzle_pos = p.get_raw<vec3>();
		const auto direction = p.get_raw<vec3>();

#ifdef JC_CLIENT
		player->set_multiple_rand_seed(rand_seed);
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, rand_seed, weapon_id, muzzle_pos, direction);
#endif

		player->fire_current_weapon(weapon_id, muzzle_pos, direction);

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

enet::PacketResult nh::player::enter_exit_vehicle(const enet::Packet& p)
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

	switch (const auto command = p.get_u8())
	{
	case VehicleEnterExit_OpenDoor:
	case VehicleEnterExit_Exit:
	{
		log(GREEN, "opening door...");

#ifdef JC_CLIENT
		const auto vehicle = vehicle_net->get_object();
		const auto seat = vehicle->get_driver_seat();
		const auto player_char = player->get_character();
#endif

		if (command == VehicleEnterExit_OpenDoor)
		{
#ifdef JC_CLIENT
			seat->open_door(player_char);
#else
			vehicle_net->set_sync_type(SyncType_Locked);
			vehicle_net->set_streamer(player);

			g_net->send_broadcast_reliable(pc, PlayerPID_EnterExitVehicle, player, vehicle_net, command);
#endif
		}
		else
		{
			log(GREEN, "exiting vehicle...");

			const bool instant = p.get_bool();

#ifdef JC_CLIENT
			seat->kick_current(instant);
#else
			vehicle_net->set_sync_type(SyncType_Distance);
			vehicle_net->set_streamer(nullptr);

			g_net->send_broadcast_reliable(pc, PlayerPID_EnterExitVehicle, player, vehicle_net, command, instant);
#endif
		}

		break;
	}
	case VehicleEnterExit_Enter:
	{
		const bool instant = p.get_bool();

		log(GREEN, "entering vehicle... {}", instant);

#ifdef JC_CLIENT
		const auto player_char = player->get_character();

		if (instant)
			player_char->hopp_into_vehicle();
		else player_char->set_enter_vehicle_stance(false);
#else
		vehicle_net->set_sync_type(SyncType_Locked);
		vehicle_net->set_streamer(player);

		g_net->send_broadcast_reliable(pc, PlayerPID_EnterExitVehicle, player, vehicle_net, command, instant);
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

	vehicle_net->set_control_info(packed_info);

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleControl, player, vehicle_net, packed_info);
#endif

	return enet::PacketRes_Ok;
}