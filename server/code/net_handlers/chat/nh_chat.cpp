#include <defs/standard.h>

#include "nh_chat.h"

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::chat::msg(const enet::PacketR& p)
{
	const auto pc = p.get_player_client_owner();
	const auto player = pc->get_player();
	const auto msg = p.get_str<std::string>();

	enet::send_broadcast_reliable<ChannelID_Chat>(ChatPID_ChatMsg, player, msg);

	logt(WHITE, "{}: {}", pc->get_nick(), msg);

	return enet::PacketRes_Ok;
}