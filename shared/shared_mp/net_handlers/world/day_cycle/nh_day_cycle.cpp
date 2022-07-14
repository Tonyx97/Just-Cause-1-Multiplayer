#include <defs/standard.h>

#include "nh_day_cycle.h"

#ifdef JC_CLIENT
#include <game/sys/day_cycle.h>
#endif

enet::PacketResult nh::day_cycle::dispatch(const enet::Packet& p)
{
#ifdef JC_CLIENT
	g_day_cycle->set_enabled(p.get_bool());
	g_day_cycle->set_time(p.get_float());
#endif

	return enet::PacketRes_Ok;
}