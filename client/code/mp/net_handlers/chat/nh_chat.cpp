#include <defs/standard.h>

#include <mp/chat/chat.h>

#include "nh_chat.h"

#include <mp/player_client/player_client.h>

enet::PacketResult nh::chat::dispatch(const enet::PacketR& p)
{
	const auto player = p.get_net_obj()->cast_safe<PlayerClient>();
	const auto message = p.get_str<std::string>();

	g_chat->add_chat_msg(player->get_nick() + ": " + message);

	return enet::PacketRes_Ok;
}