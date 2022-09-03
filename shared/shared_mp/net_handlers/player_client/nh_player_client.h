#pragma once

namespace nh::player_client
{
	PacketResult init(const Packet& p);
	PacketResult join(const Packet& p);
	PacketResult quit(const Packet& p);
	PacketResult sync_instances(const Packet& p);
	PacketResult startup_info(const Packet& p);
	PacketResult nick(const Packet& p);
}