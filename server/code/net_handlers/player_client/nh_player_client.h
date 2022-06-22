#pragma once

#include <net/interface.h>

namespace nh::player_client
{
	enet::PacketResult nick(const enet::PacketR& p);
}