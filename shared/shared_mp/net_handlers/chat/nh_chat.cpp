#include <defs/standard.h>

#include "nh_chat.h"

#include <mp/net.h>
#include <shared_mp/player_client/player_client.h>
#include <shared_mp/objs/player.h>

#ifdef JC_CLIENT
#include <mp/chat/chat.h>
#endif

enet::PacketResult nh::chat::msg(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
		g_chat->add_chat_msg(player->get_nick() + " (" + std::to_string(player->get_nid()) + "): " + p.get_str());
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
	const auto msg = p.get_str<std::string>();

	g_net->send_broadcast_reliable<ChannelID_Chat>(ChatPID_Msg, player, msg);

	logt(WHITE, "{}: {}", pc->get_nick(), msg);
#endif

	return enet::PacketRes_Ok;
}