#pragma once

#include <net/interface.h>

namespace nh::player_client
{
	enet::PacketResult init(const enet::Packet& p);
	enet::PacketResult join(const enet::Packet& p);
	enet::PacketResult quit(const enet::Packet& p);
	enet::PacketResult sync_instances(const enet::Packet& p);
	enet::PacketResult startup_info(const enet::Packet& p);
	enet::PacketResult nick(const enet::Packet& p);
}