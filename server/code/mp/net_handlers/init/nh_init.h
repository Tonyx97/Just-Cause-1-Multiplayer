#pragma once

#include <net/interface.h>

namespace nh::init
{
	enet::PacketResult init(const enet::PacketR& p);
}