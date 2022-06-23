#include <defs/standard.h>

#include "nh_check.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::check::net_objects(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto num = p.get_int<int>();

	log(YELLOW, "[{}] {}", CURR_FN, num);
#else
	const auto pc = p.get_player_client_owner();
	const auto player = pc->get_player();

	enet::send_broadcast_reliable<ChannelID_Check>(CheckPID_NetObjects, 0x1234);

	log(YELLOW, "[{}]", CURR_FN);
#endif

	return enet::PacketRes_Ok;
}