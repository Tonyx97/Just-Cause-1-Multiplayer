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
		case PlayerDynInfo_Transform:
		{
			auto position = p.get_raw<vec3>();
			auto rotation = p.get_raw<i16vec4>();

			player->set_transform(position, jc::math::unpack_quat(rotation));

#ifdef JC_SERVER
			g_net->send_broadcast_unreliable(pc, PlayerPID_DynamicInfo, player, type, position, rotation);
#endif

			break;
		}
		case PlayerDynInfo_Velocity:
		{
			auto velocity = p.get_raw<vec3>();

			player->set_velocity(velocity);

#ifdef JC_SERVER
			g_net->send_broadcast_unreliable(pc, PlayerPID_DynamicInfo, player, type, velocity);
#endif

			break;
		}
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
			const auto skin_id = p.get_u32();

			player->set_skin(skin_id);

#ifdef JC_SERVER
			g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, skin_id);
#endif

			break;
		}
		case PlayerDynInfo_Health:
		{
			const auto hp = p.get_float();

			player->set_hp(hp);

#ifdef JC_SERVER
			g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, hp);
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
		const auto target_pos = p.get_raw<vec3>();

		player->fire_current_weapon(weapon_id, muzzle_pos, target_pos);

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, weapon_id, muzzle_pos, target_pos);
#endif

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

	log(GREEN, "A player changed weapon from {} to {}", player->get_weapon_id(), weapon_id);

	player->set_weapon_id(weapon_id);

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_SetWeapon, player, weapon_id);
#endif

	return enet::PacketRes_Ok;
}