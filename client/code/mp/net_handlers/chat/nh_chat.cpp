#include <defs/standard.h>

#include <mp/chat/chat.h>

#include "nh_chat.h"

enet::PacketResult nh::chat::dispatch(const enet::PacketR& p)
{
	enet::PacketW out(ChatPID_ChatMsg);

	const auto player_name = p.get_str();
	const auto message = p.get_str();

	g_chat->add_chat_msg(player_name + ": " + message);

	return enet::PacketRes_Ok;
}