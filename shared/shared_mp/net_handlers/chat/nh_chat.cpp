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
	{
		const auto full_text = p.get_str();

		g_chat->add_chat_msg(full_text);
	}
#else
	const auto pc = p.get_pc();
	const auto msg = p.get_str();

	std::string nick_color  = "";

	if (pc->has_acl("owner"))			nick_color = "##ff0000ff";
	else if (pc->has_acl("admin"))		nick_color = "##ff8000ff";
	else if (pc->has_acl("supporter"))	nick_color = "##00ffffff";

	g_net->send_broadcast(Packet(ChatPID_Msg, ChannelID_Chat, pc->get_player(), nick_color + pc->get_nick() + "##ffffffff: " + msg));

	logt(WHITE, "{}: {}", pc->get_nick(), msg);
#endif

	return PacketRes_Ok;
}