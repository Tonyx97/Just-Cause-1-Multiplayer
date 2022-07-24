#pragma once

#include <net/interface.h>

namespace nh::player
{
	enet::PacketResult respawn(const enet::Packet& p);
	enet::PacketResult dynamic_info(const enet::Packet& p);
	enet::PacketResult stance_and_movement(const enet::Packet& p);
	enet::PacketResult set_weapon(const enet::Packet& p);
}