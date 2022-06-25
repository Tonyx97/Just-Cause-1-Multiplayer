#pragma once

#include <net/interface.h>

namespace nh::player_client
{
	enet::PacketResult state(const enet::PacketR& p);

#ifdef JC_CLIENT
	enet::PacketResult connect(const enet::PacketR& p);
	enet::PacketResult disconnect(const enet::PacketR& p);
#endif

	enet::PacketResult nick(const enet::PacketR& p);
}