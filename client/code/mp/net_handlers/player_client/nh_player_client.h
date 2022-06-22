#pragma once

#include <net/interface.h>

namespace nh::player_client
{
	enet::PacketResult connect(const enet::PacketR& p);
	enet::PacketResult disconnect(const enet::PacketR& p);
	enet::PacketResult nick(const enet::PacketR& p);
}