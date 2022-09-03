#pragma once

namespace nh::vehicle
{
	PacketResult vehicle_control(const Packet& p);
	PacketResult vehicle_honk(const Packet& p);
	PacketResult vehicle_engine_state(const Packet& p);
	PacketResult vehicle_fire(const Packet& p);
	PacketResult vehicle_mounted_gun_fire(const Packet& p);
}