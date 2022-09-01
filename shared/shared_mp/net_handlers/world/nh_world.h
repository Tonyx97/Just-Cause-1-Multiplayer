#pragma once

namespace nh::world
{
	enet::PacketResult time_scale(const enet::Packet& p);
	enet::PacketResult day_time(const enet::Packet& p);
	enet::PacketResult punch_force(const enet::Packet& p);
	enet::PacketResult spawn_object(const enet::Packet& p);
	enet::PacketResult destroy_object(const enet::Packet& p);
	enet::PacketResult set_ownership(const enet::Packet& p);
	enet::PacketResult sync_object(const enet::Packet& p);
}