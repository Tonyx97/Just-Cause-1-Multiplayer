#include <defs/standard.h>

#include <mp/net.h>

NetObject* get_net_object_from_lists(NID nid, NetObjectType type)
{
	return type != NetObject_Invalid ? g_net->get_net_object_by_nid(nid) : nullptr;
}