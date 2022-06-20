#pragma once

#include <enet/interface.h>

namespace nh::player
{
	enet::PacketResult dispatch(const enet::PacketR& p);
}