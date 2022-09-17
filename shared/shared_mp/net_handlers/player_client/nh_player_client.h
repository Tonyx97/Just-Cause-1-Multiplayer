#pragma once

namespace nh::player_client
{
	PacketResult init(const Packet& p);
	PacketResult join(const Packet& p);
	PacketResult nick(const Packet& p);
	PacketResult object_instance_sync(const Packet& p);
}