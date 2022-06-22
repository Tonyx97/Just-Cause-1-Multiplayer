#pragma once

#include <net/interface.h>

namespace nh::chat
{
	enet::PacketResult dispatch(const enet::PacketR& p);
}