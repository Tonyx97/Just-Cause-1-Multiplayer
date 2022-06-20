#pragma once

#include <enet/interface.h>

namespace nh::init
{
	enet::PacketResult init(const enet::PacketR& p);
}