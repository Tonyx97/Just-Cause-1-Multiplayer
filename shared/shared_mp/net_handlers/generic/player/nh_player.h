#pragma once

namespace nh::player
{
	PacketResult state_sync(const Packet& p);
	PacketResult respawn(const Packet& p);
	PacketResult dynamic_info(const Packet& p);
	PacketResult stance_and_movement(const Packet& p);
	PacketResult set_weapon(const Packet& p);
	PacketResult set_vehicle(const Packet& p);
	PacketResult enter_exit_vehicle(const Packet& p);
}