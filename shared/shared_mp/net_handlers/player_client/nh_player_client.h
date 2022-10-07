#pragma once

namespace nh::player_client
{
	PacketResult init(const Packet& p);
	PacketResult join(const Packet& p);
	PacketResult nick(const Packet& p);
	PacketResult object_instance_sync(const Packet& p);
	PacketResult resource_action(const Packet& p);
	PacketResult register_user(const Packet& p);
	PacketResult login_user(const Packet& p);
	PacketResult logout_user(const Packet& p);
}