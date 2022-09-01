#pragma once

namespace nh::player
{
	enet::PacketResult state_sync(const enet::Packet& p);
	enet::PacketResult respawn(const enet::Packet& p);
	enet::PacketResult dynamic_info(const enet::Packet& p);
	enet::PacketResult stance_and_movement(const enet::Packet& p);
	enet::PacketResult set_weapon(const enet::Packet& p);
	enet::PacketResult set_vehicle(const enet::Packet& p);
	enet::PacketResult enter_exit_vehicle(const enet::Packet& p);
}