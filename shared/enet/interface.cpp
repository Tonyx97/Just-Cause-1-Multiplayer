#include <defs/standard.h>

#include "interface.h"

#ifdef JC_CLIENT
#include <mp/net.h>

#include <mp/player_client/player_client.h>
#else
#include <sv/sv.h>

#include <player_client/player_client.h>
#endif

namespace enet
{
	channel_dispatch_t channel_fns[ChannelID_Max] = { nullptr };

    NetObject* PacketR::get_net_obj() const
    {
		const auto nid = get_int<NID>();
		const auto type = get_int<uint32_t>();

		// todojc - make a list of all net objects including players and other stuff like vehicles etc.

#ifdef JC_CLIENT
		return g_net->get_player_client_by_nid(nid);
#else
		return g_sv->get_player_client_by_nid(nid);
#endif
    }
}

#ifdef JC_CLIENT
ENetPeer* enet::GET_CLIENT_PEER()
{
	return g_net->get_peer();
}

ENetHost* enet::GET_CLIENT_HOST()
{
	return g_net->get_host();
}
#else
ENetHost* enet::GET_HOST()
{
	return g_sv->get_host();
}
#endif

void enet::init()
{
#ifdef JC_CLIENT
	//
#else
	enet::INIT_NIDS_POOL();
#endif
}

void enet::add_channel_dispatcher(uint8_t id, const channel_dispatch_t& fn)
{
	channel_fns[id] = fn;
}

void enet::call_channel_dispatcher(const ENetEvent& e)
{
	const auto channel = e.channelID;

	check(channel >= 0 && channel < ChannelID_Max, "Invalid channel ID");

	PacketR p(e);

	switch (channel_fns[channel](p))
	{
	case PacketRes_NotFound: logt(RED, "Unknown packet received, id = '{}', channel = '{}'", p.get_id(), p.get_channel()); return;
	}
}