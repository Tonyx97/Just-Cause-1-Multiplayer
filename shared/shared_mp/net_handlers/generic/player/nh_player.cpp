#include <defs/standard.h>

#include "nh_player.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

// debug

#ifdef JC_CLIENT
#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/base/comps/physical.h>
#endif

enet::PacketResult nh::player::spawn(const enet::Packet& p)
{
#ifdef JC_CLIENT
	// todojc

	/*if (const auto player = p.get_net_object<Player>())
		player->spawn();*/
#else
	// maybe we need this at some point (receive spawn packet from client)
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::tick_info(const enet::Packet& p)
{
#ifdef JC_CLIENT
	/*if (const auto player = p.get_net_object<Player>())
	{
		const auto info = p.get_struct<Player::TickInfo>();

		player->set_tick_info(info);
	}*/
#else
	/*const auto pc = p.get_pc();
	const auto player = pc->get_player();
	const auto info = p.get_struct<Player::TickInfo>();

	player->set_tick_info(info);

	g_net->send_broadcast_reliable(pc, PlayerPID_TickInfo, player, info);*/
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

	switch (const auto type = p.get_uint())
	{
	case 0u: // transform
	{
		auto transform = p.get_raw<Transform>();

#ifdef JC_CLIENT
		player->set_transform(transform);
#endif

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, transform);
#endif

		break;
	}
	case 1u: // velocity
	{
		const auto velocity = p.get_raw<vec3>();

#ifdef JC_CLIENT
		player->get_character()->get_physical()->set_velocity(velocity);
#endif

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, velocity);
#endif

		break;
	}
	case 2u: // head rotation
	{
		const auto rotation = p.get_raw<vec3>();

#ifdef JC_CLIENT
		player->get_character()->get_skeleton()->set_head_euler_rotation(rotation);
#endif

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, rotation);
#endif

		break;
	}
	case 3u: // skin
	{
		const auto skin_id = p.get_uint();

#ifdef JC_CLIENT
		player->set_skin(skin_id);
#endif

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_DynamicInfo, player, type, skin_id);
#endif
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

	switch (const auto type = p.get_uint())
	{
	case 0u: // WASD movement
	{
		const float angle = p.get_float(),
					right = p.get_float(),
					forward = p.get_float();

		const bool aiming = p.get_bool();

		//log(GREEN, "{} {} {} {}", angle, right, forward, aiming);

		player->set_movement_info(angle, right, forward, aiming);

#ifdef JC_SERVER
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, angle, right, forward, aiming);
#endif

		break;
	}
	case 1u: // jump
	{
#ifdef JC_CLIENT
		jc::hooks::call<jc::character::hook::set_body_stance_t>(player->get_character()->get_body_stance(), BodyStance_Jump);
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type);
#endif

		break;
	}
	case 2u: // punch
	{
#ifdef JC_CLIENT
		jc::hooks::call<jc::character::hook::setup_punch_t>(player->get_character());
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type);
#endif

		break;
	}
	case 3u: // jump
	{
		const auto stance_id = p.get_uint();

#ifdef JC_CLIENT
		//log(GREEN, "some player stanced: {}", stance_id);

		jc::hooks::call<jc::character::hook::set_body_stance_t>(player->get_character()->get_body_stance(), stance_id);
#else
		g_net->send_broadcast_reliable(pc, PlayerPID_StanceAndMovement, player, type, stance_id);
#endif

		break;
	}
	}

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player::health(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const float new_hp = p.get_float();

#ifdef JC_CLIENT
	player->set_hp(new_hp);
#else
	g_net->send_broadcast_reliable(pc, PlayerPID_Health, player, new_hp);
#endif

	return enet::PacketRes_Ok;
}