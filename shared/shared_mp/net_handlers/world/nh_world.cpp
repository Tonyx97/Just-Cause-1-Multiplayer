#include <defs/standard.h>

#include "nh_world.h"

#include <mp/net.h>

#ifdef JC_CLIENT
#include <game/sys/world/day_cycle.h>
#include <game/sys/time/time_system.h>
#endif

enet::PacketResult nh::world::time_scale(const enet::Packet& p)
{
	const float time_scale = p.get_float();

#ifdef JC_CLIENT
	g_time->set_time_scale(time_scale);
#else
	g_net->get_settings().set_time_scale(time_scale);

	g_net->send_broadcast_joined_reliable<ChannelID_World>(WorldPID_SetTimeScale, time_scale);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::world::day_time(const enet::Packet& p)
{
#ifdef JC_CLIENT
	g_day_cycle->set_enabled(p.get_bool());
	g_day_cycle->set_time(p.get_float());
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::world::spawn_object(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto [nid, net_type] = p.get_nid_and_type();
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();

	const auto net_type = p.get_integral<NetObjectType>();
#endif

	const auto object_id = p.get_u16();
	const auto transform = p.get_raw<TransformTR>();

#ifdef JC_CLIENT
	const auto obj = g_net->spawn_net_object(nid, net_type, object_id, transform);
#else
	const auto obj = g_net->spawn_net_object(SyncType_Distance, net_type, object_id, transform);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::world::destroy_object(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
#endif

	const auto net_obj = p.get_net_object();

	if (!net_obj)
		return enet::PacketRes_BadArgs;

	log(GREEN, "A net object with nid was destroyed, type {}", net_obj->get_nid(), net_obj->get_type());

	g_net->destroy_net_object(net_obj);

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::world::set_ownership(const enet::Packet& p)
{
#ifdef JC_CLIENT
	Player* new_streamer = nullptr;

	if (const auto acquire_ownership = p.get_bool())
		new_streamer = p.get_net_object<Player>();

	const auto net_obj = p.get_net_object();

	if (!net_obj)
		return enet::PacketRes_BadArgs;

	net_obj->set_streamer(new_streamer);

	return enet::PacketRes_Ok;
#endif

	return enet::PacketRes_NotSupported;
}

enet::PacketResult nh::world::sync_object(const enet::Packet& p)
{
	const auto net_obj = p.get_net_object();

	if (!net_obj)
		return enet::PacketRes_BadArgs;

#ifdef JC_SERVER
	const auto pc = p.get_pc();
	const auto player = pc->get_player();

	// check if the player sending this packet is actually streaming
	// the object

	if (net_obj->get_type() != NetObject_Player)
	{
		if (!net_obj->is_owned_by(player))
			return enet::PacketRes_NotAllowed;
	}
	else if (net_obj != player)
		return enet::PacketRes_NotAllowed;
#endif

	switch (const auto var_type = p.get_u8())
	{
	case NetObjectVar_Transform:
	{
		const auto transform = p.get_raw<TransformPackedTR>();

		net_obj->set_transform(transform);

#ifdef JC_SERVER
		g_net->send_broadcast_joined_unreliable<ChannelID_World>(pc, WorldPID_SyncObject, net_obj, NetObjectVar_Transform, transform);
#endif

		break;
	}
	case NetObjectVar_Velocity:
	{
		const auto velocity = p.get_raw<vec3>();

		net_obj->set_velocity(velocity);

#ifdef JC_SERVER
		g_net->send_broadcast_joined_unreliable<ChannelID_World>(pc, WorldPID_SyncObject, net_obj, NetObjectVar_Velocity, velocity);
#endif

		break;
	}
	case NetObjectVar_Health:
	{
		const auto hp = p.get_float();

		net_obj->set_hp(hp);

#ifdef JC_SERVER
		g_net->send_broadcast_joined_reliable<ChannelID_World>(pc, WorldPID_SyncObject, net_obj, NetObjectVar_Health, hp);
#endif

		break;
	}
	case NetObjectVar_MaxHealth:
	{
		const auto max_hp = p.get_float();

		net_obj->set_max_hp(max_hp);

#ifdef JC_SERVER
		g_net->send_broadcast_joined_reliable<ChannelID_World>(pc, WorldPID_SyncObject, net_obj, NetObjectVar_MaxHealth, max_hp);
#endif

		break;
	}
	default: log(RED, "Unknown net object var type: {}", var_type);
	}

	return enet::PacketRes_Ok;
}