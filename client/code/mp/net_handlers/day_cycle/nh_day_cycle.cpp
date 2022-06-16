#include <defs/standard.h>

#include "nh_day_cycle.h"

#include <game/sys/day_cycle.h>

enet::PacketResult nh::day_cycle::dispatch(const enet::PacketR& p)
{
	g_day_cycle->set_time(p.get<float>());

	return enet::PacketRes_Ok;
}