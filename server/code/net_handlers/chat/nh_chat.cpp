#include <defs/standard.h>

#include "nh_chat.h"

enet::PacketResult nh::chat::dispatch(const enet::PacketR& p)
{
	const auto player_name = p.get_str();
	const auto message = p.get_str();

	enet::PacketW out(ChatPID_ChatMsg);

	out.add(player_name);
	out.add(message);
	out.send_broadcast(ChannelID_Chat);

	logt(WHITE, "{}: {}", player_name, message);

	return enet::PacketRes_Ok;
}