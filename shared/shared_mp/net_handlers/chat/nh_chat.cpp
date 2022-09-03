#include <defs/standard.h>

#include <mp/net.h>

#include "nh_chat.h"

#include <shared_mp/player_client/player_client.h>
#include <shared_mp/objs/player.h>

#ifdef JC_CLIENT
#include <mp/chat/chat.h>
#endif

PacketResult nh::chat::msg(const Packet& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
		g_chat->add_chat_msg(player->get_nick() + ": " + p.get_str());
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
	const auto msg = p.get_str();

	p.add_beginning(player);

	g_net->send_broadcast(p);

	logt(WHITE, "{}: {}", pc->get_nick(), msg);
#endif

	return PacketRes_Ok;
}