#include <defs/standard.h>

#include <mp/net.h>

#include "blip_net.h"

#ifdef JC_CLIENT
#include <game/sys/core/factory_system.h>

#include <game/object/ui/map_icon.h>

BlipNetObject::BlipNetObject(NID nid, const TransformTR& transform)
{
	set_nid(nid);
	set_transform(transform);
}

ObjectBase* BlipNetObject::get_object_base()
{
	return obj;
}
#else
BlipNetObject::BlipNetObject(SyncType sync_type, const TransformTR& transform)
{
	set_sync_type(sync_type);
	set_transform(transform);
}
#endif

BlipNetObject::~BlipNetObject()
{
#ifdef JC_CLIENT
	g_factory->destroy_map_icon(obj);
#endif
}

void BlipNetObject::on_sync()
{
}

void BlipNetObject::on_net_var_change(NetObjectVarType var_type)
{
#ifdef JC_CLIENT
#else
	switch (var_type)
	{
	case NetObjectVar_Position:
	case NetObjectVar_Rotation:
	case NetObjectVar_Transform: g_net->send_broadcast_joined_reliable<ChannelID_World>(WorldPID_SyncObject, this, NetObjectVar_Transform, get_transform()); break;
	default:
		break;
	}
#endif
}

bool BlipNetObject::spawn()
{
	// if it's already spawned then do nothing

	if (is_spawned())
	{
		log(RED, "BlipNetObject {:x} was already spawned, where are you calling this from?", get_nid());

		return false;
	}

#ifdef JC_CLIENT
	obj = g_factory->create_map_icon("icon_interestpoint_collect", get_position());

	check(obj, "Could not create blip object");

	log(PURPLE, "{} {:x} spawned now {:x} at {:.2f} {:.2f} {:.2f}", typeid(*obj).name(), get_nid(), ptr(obj), get_position().x, get_position().y, get_position().z);
#endif

	set_spawned(true);

	return true;
}