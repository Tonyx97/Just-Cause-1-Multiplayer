#pragma once

#include <net/interface.h>

namespace nh::check
{
	enet::PacketResult net_objects(const enet::PacketR& p);
}