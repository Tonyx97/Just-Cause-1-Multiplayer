#pragma once

namespace nh::player_client
{
	PacketResult init(const Packet& p);
	PacketResult join(const Packet& p);
	PacketResult nick(const Packet& p);
	PacketResult object_instance_sync(const Packet& p);
	PacketResult ownerships(const Packet& p);
	PacketResult resource_action(const Packet& p);
	PacketResult resources_refresh(const Packet& p);
	PacketResult register_user(const Packet& p);
	PacketResult login_user(const Packet& p);
	PacketResult logout_user(const Packet& p);
	PacketResult debug_log(const Packet& p);
	PacketResult debug_enable_admin_panel(const Packet& p);
	PacketResult trigger_remote_event(const Packet& p);
}