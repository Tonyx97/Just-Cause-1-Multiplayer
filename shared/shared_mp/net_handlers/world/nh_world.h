#pragma once

#include <net/interface.h>

namespace nh::world
{
	enet::PacketResult day_time(const enet::Packet& p);
	enet::PacketResult spawn_object(const enet::Packet& p);
}