#pragma once

#include <enet/interface.h>

namespace nh::day_cycle
{
	enet::PacketResult dispatch(const enet::PacketR& p);
}