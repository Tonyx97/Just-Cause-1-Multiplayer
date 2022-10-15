#include <defs/standard.h>

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

#include "nh_world.h"

#ifdef JC_CLIENT
#include <game/object/character/character.h>

#include <game/sys/world/day_cycle.h>
#include <game/sys/time/time_system.h>
#endif

PacketResult nh::world::time_scale(const Packet& p)
{
	const float time_scale = p.get_float();

#ifdef JC_CLIENT
	g_time->set_time_scale(time_scale);
#else
	g_net->get_settings().set_time_scale(time_scale);
#endif

	return PacketRes_Ok;
}

PacketResult nh::world::day_time(const Packet& p)
{
#ifdef JC_CLIENT
	g_day_cycle->set_enabled(p.get_bool());
	g_day_cycle->set_time(p.get_float());
#endif

	return PacketRes_Ok;
}

PacketResult nh::world::punch_force(const Packet& p)
{
	const float force = p.get_float();

#ifdef JC_CLIENT
	Character::SET_GLOBAL_PUNCH_DAMAGE(force);
	Character::SET_GLOBAL_PUNCH_DAMAGE(force, true);
#else
	g_net->get_settings().set_punch_force(force);
#endif

	return PacketRes_Ok;
}

PacketResult nh::world::spawn_object(const Packet& p, bool with_pfx)
{
#ifdef JC_SERVER
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
	const auto net_type = p.get<NetObjectType>();
	const auto object_id = p.get_str();

	std::string pfx_id;

	if (with_pfx)
		pfx_id = p.get_str();

	const auto transform = p.get<TransformTR>();

	g_net->spawn_net_object(SyncType_Distance, net_type, object_id, pfx_id, transform);
#endif

	return PacketRes_Ok;
}

PacketResult nh::world::destroy_object(const Packet& p)
{
#ifdef JC_CLIENT
#else
#endif

	const auto net_obj = p.get_net_object();

	if (!net_obj)
		return PacketRes_BadArgs;

	log(GREEN, "A net object with nid {:x} was destroyed, type {}", net_obj->get_nid(), net_obj->get_type());

	g_net->destroy_net_object(net_obj);

	return PacketRes_Ok;
}

PacketResult nh::world::sync_object(const Packet& p)
{
	const auto net_obj = p.get_net_object();

	if (!net_obj)
		return PacketRes_BadArgs;

#ifdef JC_SERVER
	const auto pc = p.get_pc();
	const auto player = pc->get_player();

	// check if the player sending this packet is actually streaming
	// the object

	if (net_obj->get_type() != NetObject_Player)
	{
		if (!net_obj->is_owned_by(player))
			return PacketRes_NotAllowed;
	}
	else if (net_obj != player)
		return PacketRes_NotAllowed;
#endif

	// make sure we resend the packet to all clients
	// because the setters won't send any packets

	// packets received from clients are special and they use the reliability the
	// client used, we are just broadcasting the var to all players

	switch (const auto var_type = p.get_u8())
	{
	case NetObjectVar_Transform:
	{
		const auto transform = p.get<TransformPackedTR>();

		net_obj->set_transform(transform);

		break;
	}
	case NetObjectVar_Velocity:
	{
		const auto velocity = p.get<vec3>();

		net_obj->set_velocity(velocity);

		break;
	}
	case NetObjectVar_Health:
	{
		const auto hp = p.get_float();

		net_obj->set_hp(hp, true);

		break;
	}
	case NetObjectVar_MaxHealth:
	{
		const auto max_hp = p.get_float();

		net_obj->set_max_hp(max_hp);

		break;
	}
	default: log(RED, "Unknown net object var type: {}", var_type);
	}

#ifdef JC_SERVER
	g_net->send_broadcast_joined(pc, p);
#endif

	return PacketRes_Ok;
}