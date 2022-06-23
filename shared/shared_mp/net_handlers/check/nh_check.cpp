#include <defs/standard.h>

#include "nh_check.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::check::net_objects(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto count = p.get_int<int>();

	log(YELLOW, "[{}] {}", CURR_FN, count);

	for (int i = 0; i < count; ++i)
	{
		const auto nid = p.get_int<NID>();
		const auto type = p.get_int<uint32_t>();

		if (g_net->get_net_object_by_nid(nid))
		{
			log(YELLOW, "Net object with type {} and NID {:x} already exists", type, nid);

			continue;
		}

		switch (type)
		{
		case NetObject_Player:
		{
			log(YELLOW, "New player added with NID {:x}", nid);

			g_net->add_player_client(nid);
		}
		default:
			log(RED, "Unknown type of net object at '{}'", CURR_FN);
		}
	}
#else
	enet::PacketW out_p(CheckPID_NetObjects);

	out_p.add(g_net->get_net_objects_count());

	g_net->for_each_net_object([&](NID nid, NetObject* obj)
	{
		out_p.add(obj);
	});

	out_p.send(p.get_peer(), ChannelID_Check);
#endif

	return enet::PacketRes_Ok;
}