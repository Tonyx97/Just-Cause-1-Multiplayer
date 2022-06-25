#pragma once

#include <net/interface.h>

namespace nh::player
{
	enet::PacketResult sync_spawn(const enet::PacketR& p);
	enet::PacketResult transform(const enet::PacketR& p);
	enet::PacketResult set_anim(const enet::PacketR& p);
}