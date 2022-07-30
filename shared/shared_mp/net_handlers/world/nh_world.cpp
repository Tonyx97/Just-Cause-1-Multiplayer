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