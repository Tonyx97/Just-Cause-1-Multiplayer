#pragma once

#include <net/interface.h>

namespace nh::player
{
	enet::PacketResult spawn(const enet::PacketR& p);
	enet::PacketResult tick_info(const enet::PacketR& p);
}