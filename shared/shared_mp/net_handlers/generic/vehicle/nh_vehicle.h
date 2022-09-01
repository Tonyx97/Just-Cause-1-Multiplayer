#pragma once

namespace nh::vehicle
{
	enet::PacketResult vehicle_control(const enet::Packet& p);
	enet::PacketResult vehicle_honk(const enet::Packet& p);
	enet::PacketResult vehicle_engine_state(const enet::Packet& p);
	enet::PacketResult vehicle_fire(const enet::Packet& p);
	enet::PacketResult vehicle_mounted_gun_fire(const enet::Packet& p);
}