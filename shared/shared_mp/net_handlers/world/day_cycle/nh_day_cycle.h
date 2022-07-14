#pragma once

#include <net/interface.h>

namespace nh::day_cycle
{
	enet::PacketResult dispatch(const enet::Packet& p);
}