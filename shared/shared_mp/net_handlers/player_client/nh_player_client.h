#pragma once

#include <net/interface.h>

namespace nh::player_client
{
	enet::PacketResult init(const enet::PacketR& p);
	enet::PacketResult join(const enet::PacketR& p);
	enet::PacketResult quit(const enet::PacketR& p);
	enet::PacketResult sync_instances(const enet::PacketR& p);
	enet::PacketResult nick(const enet::PacketR& p);
}