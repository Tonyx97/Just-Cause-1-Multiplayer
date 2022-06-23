#pragma once

#include <net/interface.h>

namespace nh::chat
{
	enet::PacketResult msg(const enet::PacketR& p);
}