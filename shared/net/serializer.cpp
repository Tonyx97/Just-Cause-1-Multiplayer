#include <defs/standard.h>

#include "serializer.h"

#include <mp/net.h>

NetObject* enet::deserialize_net_object(std::vector<uint8_t>& data)
{
	const auto nid = deserialize_int(data);

	if (nid != INVALID_NID)
	{
		const auto type = deserialize_int<NetObjectType>(data);

		return type != NetObject_Invalid ? g_net->get_net_object_by_nid(nid) : nullptr;
	}

	return nullptr;
}