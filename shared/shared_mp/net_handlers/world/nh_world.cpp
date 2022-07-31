#include <defs/standard.h>

#include "nh_world.h"

#include <mp/net.h>

#ifdef JC_CLIENT
#include <game/sys/day_cycle.h>
#endif

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
#ifdef JC_SERVER
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto position = p.get_raw<vec3>();

#ifdef JC_CLIENT
	const auto [nid, type] = p.get_nid_and_type();

	switch (type)
	{
	case NetObject_Damageable:
	{
		log(GREEN, "Server spawned damageable: NID = {:x}, position = {:.2f} {:.2f} {:.2f}", nid, position.x, position.y, position.z);

		g_net->spawn_damageable(nid, position);

		break;
	}
	default: log(RED, "Unknown net object type to spawn: {}", type);
	}
#else
	const auto net_type = p.get_integral<NetObjectType>();

	switch (net_type)
	{
	case NetObject_Damageable:
	{
		const auto object = g_net->spawn_damageable(position);

		log(GREEN, "Spawning damageable: owner = {:x}, NID = {:x}, position = {:.2f} {:.2f} {:.2f}", ptr(player), object->get_nid(), position.x, position.y, position.z);

		break;
	}
	default: log(RED, "Unknown net object type to spawn: {}", net_type);
	}
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::world::set_ownership(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto new_streamer = p.get_net_object<Player>();
	const auto net_obj = p.get_net_object();

	net_obj->set_streamer(new_streamer);

	return enet::PacketRes_Ok;
#endif

	return enet::PacketRes_NotSupported;
}

enet::PacketResult nh::world::sync_object(const enet::Packet& p)
{
	const auto initial_data = p.get_initial_data();
	
	const auto net_obj = p.get_net_object();

	check(net_obj, "Invalid net object");

#ifdef JC_SERVER
	const auto pc = p.get_pc();
	const auto player = pc->get_player();

	if (!net_obj->is_owned_by(player))
		return enet::PacketRes_NotAllowed;
#endif

	while (!p.is_empty())
	{
		switch (const auto var_type = p.get_u8())
		{
		case NetObjectVar_Transform:
		{
			const auto transform = p.get_raw<TransformTR>();

			net_obj->set_transform(transform.t, transform.r);

			break;
		}
		case NetObjectVar_Health:
		{
			const auto hp = p.get_float();

			net_obj->set_hp(hp);

			log(GREEN, "hp: {}", net_obj->get_hp());

			break;
		}
		case NetObjectVar_MaxHealth:
		{
			const auto max_hp = p.get_float();

			net_obj->set_max_hp(max_hp);

			log(GREEN, "max_hp: {}", net_obj->get_max_hp());

			break;
		}
		default: log(RED, "Unknown net object var type: {}", var_type);
		}
	}

#ifdef JC_SERVER
	// send the whole data we got from the packet directly to the rest of the players

	g_net->send_broadcast_unreliable<ChannelID_World>(pc, initial_data);
#endif

	return enet::PacketRes_Ok;
}