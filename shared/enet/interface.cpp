#include <defs/standard.h>

#include "interface.h"

namespace enet
{
	channel_dispatch_t channel_fns[ChannelID_Max] = { nullptr };
}

#ifdef JC_CLIENT
#include <mp/net.h>

ENetPeer* enet::GET_CLIENT_PEER()
{
	return g_net->get_peer();
}

ENetHost* enet::GET_CLIENT_HOST()
{
	return g_net->get_host();
}
#else
#include <sv/sv.h>

ENetHost* enet::GET_HOST()
{
	return g_sv->get_host();
}
#endif

void enet::add_channel_dispatcher(uint8_t id, const channel_dispatch_t& fn)
{
	channel_fns[id] = fn;
}

void enet::call_channel_dispatcher(const ENetEvent& e)
{
	const auto id = e.channelID;

	check(id >= 0 && id < ChannelID_Max, "Invalid channel ID");

	PacketR p(e);

	switch (channel_fns[id](p))
	{
	case PacketRes_NotFound: logt(RED, "Unknown packet received, id = '{}', channel = '{}'", id, p.get_channel()); return;
	}
}