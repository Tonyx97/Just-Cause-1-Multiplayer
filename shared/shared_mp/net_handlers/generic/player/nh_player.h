#pragma once

#include <net/interface.h>

namespace nh::player
{
	enet::PacketResult spawn(const enet::Packet& p);
	enet::PacketResult dynamic_info(const enet::Packet& p);
	enet::PacketResult stance_and_movement(const enet::Packet& p);
	enet::PacketResult health(const enet::Packet& p);
}