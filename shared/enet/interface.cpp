#include <defs/standard.h>

#ifdef JC_CLIENT
#include <mp/player_client/player_client.h>
#else
#include <player_client/player_client.h>
#endif

namespace enet
{
	channel_dispatch_t channel_fns[ChannelID_Max] = { nullptr };

#ifdef JC_SERVER
	std::unordered_set<NID> free_nids,
							used_nids;
#endif

    void PacketW::add(PlayerClient* pc)
    {
		add(pc->get_nid());
    }
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

NID enet::GET_FREE_NID()
{
	check(!free_nids.empty(), "No more free NIDs available");

	const auto it = free_nids.begin();
	const auto nid = *it;

	free_nids.erase(it);
	used_nids.insert(nid);

    return nid;
}

void enet::FREE_NID(NID nid)
{
	auto it = used_nids.find(nid);

	check(it != used_nids.end(), "NID {} not used", nid);

	used_nids.erase(it);
	free_nids.insert(nid);
}
#endif

void enet::init()
{
#ifdef JC_CLIENT
	//
#else
	// create a total of 2048 possible nids (1-2048)

	for (NID i = 1u; i < 2048u + 1u; ++i)
		free_nids.insert(free_nids.end(), i);
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