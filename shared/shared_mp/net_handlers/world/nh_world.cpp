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
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto net_type = p.get_integral<NetObjectType>();

#ifdef JC_CLIENT
	switch (net_type)
	{
	case NetObject_Damageable:
	{
		log(GREEN, "Server wants to spawn a damageable");

		break;
	}
	default: log(RED, "Unknown net object type to spawn: {}", net_type);
	}
#else
	switch (net_type)
	{
	case NetObject_Damageable:
	{
		log(GREEN, "A player wants to spawn a damageable");

		g_net->send_broadcast_reliable<ChannelID_World>(WorldPID_SpawnObject, player, net_type);

		break;
	}
	default: log(RED, "Unknown net object type to spawn: {}", net_type);
	}
#endif

	return enet::PacketRes_Ok;
}