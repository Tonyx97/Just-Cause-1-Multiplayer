#include <defs/standard.h>

#ifdef JC_CLIENT
#include <mp/chat/chat.h>
#endif

#include "nh_chat.h"

#include <shared_mp/player_client/player_client.h>
#include <shared_mp/objs/player.h>

enet::PacketResult nh::chat::msg(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();
	const auto message = p.get_str<std::string>();

	g_chat->add_chat_msg(player->get_client()->get_nick() + ": " + message);
#else
	const auto pc = p.get_player_client_owner();
	const auto player = pc->get_player();
	const auto msg = p.get_str<std::string>();

	enet::send_broadcast_reliable<ChannelID_Chat>(ChatPID_ChatMsg, player, msg);

	logt(WHITE, "{}: {}", pc->get_nick(), msg);
#endif

	return enet::PacketRes_Ok;
}