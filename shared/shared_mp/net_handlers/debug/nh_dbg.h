#pragma once

#include <net/interface.h>

namespace nh::dbg
{
	enet::PacketResult set_time(const enet::Packet& p);
}