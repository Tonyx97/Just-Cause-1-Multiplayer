#include <defs/standard.h>

#include "interface.h"

#include <mp/net.h>

#include <shared_mp/objs/all.h>

namespace enet
{
	channel_dispatch_t channel_fns[ChannelID_Max] = { nullptr };

	void PacketR::get_data(void* out, size_t out_size) const
	{
		memcpy(out, data + std::exchange(offset, offset + out_size), out_size);
	}

	NetObject* PacketR::get_net_object_impl() const
    {
		const auto nid = get_uint();
		const auto type = get_uint();

		return g_net->get_net_object_by_nid(nid);
    }
}

#ifdef JC_CLIENT
ENetPeer* enet::GET_CLIENT_PEER()
{
	return g_net->get_peer();
}
#endif

ENetHost* enet::GET_HOST()
{
	return g_net->get_host();
}

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