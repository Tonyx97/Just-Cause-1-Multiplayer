#pragma once

namespace nh::vehicle
{
	PacketResult control(const Packet& p);
	PacketResult honk(const Packet& p);
	PacketResult sirens(const Packet& p);
	PacketResult fire(const Packet& p);
	PacketResult mounted_gun_fire(const Packet& p);
	PacketResult dynamic_info(const Packet& p);
}