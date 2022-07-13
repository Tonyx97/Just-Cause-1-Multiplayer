#ifndef JC_LOADER
#include <defs/standard.h>

#include "net_object.h"

#ifdef JC_SERVER
namespace enet
{
	std::unordered_set<NID> free_nids,
							used_nids;

	void INIT_NIDS_POOL()
	{
		// create a total of 2048 possible nids (1-2048)

		for (NID i = 1u; i < 2048u + 1u; ++i)
			free_nids.insert(free_nids.end(), i);
	}

	NID GET_FREE_NID()
	{
		check(!free_nids.empty(), "No more free NIDs available");

		const auto it = free_nids.begin();
		const auto nid = *it;

		free_nids.erase(it);
		used_nids.insert(nid);

		return nid;
	}

	void FREE_NID(NID nid)
	{
		auto it = used_nids.find(nid);

		check(it != used_nids.end(), "NID {} not used", nid);

		used_nids.erase(it);
		free_nids.insert(nid);
	}
}

NetObject::NetObject()
{
	nid = enet::GET_FREE_NID();
}

NetObject::~NetObject()
{
	enet::FREE_NID(nid);
}
#endif
#endif