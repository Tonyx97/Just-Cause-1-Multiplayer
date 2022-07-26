#include <defs/standard.h>

#include "interface.h"

#include <mp/net.h>

#include <shared_mp/objs/all.h>

namespace enet
{
	channel_dispatch_t channel_fns[ChannelID_Max] = { nullptr };

	std::vector<Packet> lagged_packets;

	int fake_lag = 0;

	void call_channel_dispatcher_internal(const Packet& p)
	{
		switch (channel_fns[p.get_channel()](p))
		{
		case PacketRes_NotFound: logt(RED, "Unknown packet received, id = '{}', channel = '{}'", p.get_id(), p.get_channel()); return;
		}
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

#ifdef JC_DBG
	enet::set_fake_lag(0);
#endif
}

void enet::add_channel_dispatcher(uint8_t id, const channel_dispatch_t& fn)
{
	channel_fns[id] = fn;
}

void enet::call_channel_dispatcher(const ENetEvent& e)
{
	check(e.channelID >= 0 && e.channelID < ChannelID_Max, "Invalid channel ID");

	Packet p(e);

#ifdef JC_CLIENT
	if (fake_lag <= 0)
#endif
		call_channel_dispatcher_internal(p);
#ifdef JC_CLIENT
	else
	{
		// if we are using fake lag then accumulate the packets to execute them later
		// available in client only for now

		lagged_packets.push_back(std::move(p));
	}
#endif
}

void enet::set_fake_lag(int value)
{
	fake_lag = value;
}

void enet::process_lagged_packets()
{
#ifdef JC_CLIENT
	if (fake_lag <= 0)
		return;

	const auto curr_time = util::time::get_time();

	for (auto it = lagged_packets.begin(); it != lagged_packets.end();)
	{
		auto& p = *it;

		const bool remove = p.get_time() < curr_time - fake_lag;

		if (remove)
		{
			call_channel_dispatcher_internal(p);

			it = lagged_packets.erase(it);
		}
		else ++it;
	}
#endif
}

int enet::get_fake_lag()
{
	return fake_lag;
}