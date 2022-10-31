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

UIMapIcon* BlipNetObject::get_object() const
{
	return obj.get();
}

ObjectBase* BlipNetObject::get_object_base() const
{
	return get_object();
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
	destroy_object();
}

void BlipNetObject::destroy_object()
{
	IF_CLIENT_AND_VALID_OBJ_DO([&]() { obj.reset(); });
}

void BlipNetObject::on_sync()
{
}

void BlipNetObject::on_spawn()
{
#ifdef JC_CLIENT
	obj = g_factory->create_map_icon("icon_interestpoint_collect", get_position());

	check(obj, "Could not create blip object");

	log(PURPLE, "BlipNetObject {:x} spawned now {:x} at {:.2f} {:.2f} {:.2f}", get_nid(), ptr(obj.get()), get_position().x, get_position().y, get_position().z);
#endif
}

void BlipNetObject::on_despawn()
{
	destroy_object();
}

void BlipNetObject::on_net_var_change(NetObjectVarType var_type)
{
#ifdef JC_CLIENT
	IF_CLIENT_AND_VALID_OBJ_DO([&]()
	{
		switch (var_type)
		{
		case NetObjectVar_Transform:
		case NetObjectVar_Position:
		case NetObjectVar_Rotation:		obj->set_transform(Transform(get_position(), get_rotation())); break;
		}
	});
#else
#endif
}