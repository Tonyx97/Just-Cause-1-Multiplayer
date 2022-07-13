#pragma once

#include <net/interface.h>

namespace nh::player
{
	enet::PacketResult spawn(const enet::Packet& p);
	enet::PacketResult tick_info(const enet::Packet& p);
	enet::PacketResult dynamic_info(const enet::Packet& p);
	enet::PacketResult stance(const enet::Packet& p);
}