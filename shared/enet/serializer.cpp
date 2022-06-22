#ifndef JC_LOADER
#include <defs/standard.h>

#include "serializer.h"
#include "net_obj.h"

void enet::serialize_net_object(vec<uint8_t>& buffer, NetObject* v)
{
	serialize(buffer, v->get_nid());
	serialize(buffer, v->get_type());
}
#endif