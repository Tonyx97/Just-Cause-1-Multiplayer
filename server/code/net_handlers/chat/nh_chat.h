#pragma once

#include <enet/interface.h>

namespace nh::chat
{
	enet::PacketResult msg(const enet::PacketR& p);
}