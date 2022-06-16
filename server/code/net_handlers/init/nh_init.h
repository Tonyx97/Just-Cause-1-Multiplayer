#pragma once

#include <enet/interface.h>

namespace nh::init
{
	enet::PacketResult dispatch(const enet::PacketR& p);
}