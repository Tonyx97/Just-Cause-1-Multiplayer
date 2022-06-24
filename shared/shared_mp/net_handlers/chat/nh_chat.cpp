#include <defs/standard.h>

#include "nh_chat.h"

#include <shared_mp/player_client/player_client.h>
#include <shared_mp/objs/player.h>

#ifdef JC_CLIENT
#include <mp/chat/chat.h>
#endif

enet::PacketResult nh::chat::msg(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
		g_chat->add_chat_msg(player->get_nick() + ": " + p.get_str<std::string>());
#else
	const auto pc = p.get_player_client_owner();
	const auto player = pc->get_player();
	const auto msg = p.get_str<std::string>();

	enet::send_broadcast_reliable<ChannelID_Chat>(ChatPID_ChatMsg, player, msg);

	logt(WHITE, "{}: {}", pc->get_nick(), msg);
#endif

	return enet::PacketRes_Ok;
}