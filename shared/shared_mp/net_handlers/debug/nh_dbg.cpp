#include <defs/standard.h>

#include "nh_dbg.h"

#include <mp/net.h>
#include <shared_mp/player_client/player_client.h>
#include <shared_mp/objs/player.h>

enet::PacketResult nh::dbg::set_time(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
	g_net->get_settings().set_day_time(p.get_float());
#endif

	return enet::PacketRes_Ok;
}