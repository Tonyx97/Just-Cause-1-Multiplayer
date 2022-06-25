#include <defs/standard.h>

#include "serializer.h"

void enet::serialize_net_object(vec<uint8_t>& buffer, size_t at, NetObject* v)
{
	serialize(buffer, at, v->get_nid());
	serialize(buffer, at, v->get_type());
}