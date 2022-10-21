#include <defs/standard.h>

#include <mp/net.h>

#include "pickup_net.h"

#ifdef JC_CLIENT
#include <game/sys/core/factory_system.h>
#include <game/object/item/item_pickup.h>

PickupNetObject::PickupNetObject(NID nid, const TransformTR& transform)
{
	set_nid(nid);
	set_transform(transform);
}

ObjectBase* PickupNetObject::get_object_base() const
{
	return obj;
}
#else
PickupNetObject::PickupNetObject(SyncType sync_type, const TransformTR& transform)
{
	set_sync_type(sync_type);
	set_transform(transform);
}
#endif

PickupNetObject::~PickupNetObject()
{
	destroy_object();
}

void PickupNetObject::destroy_object()
{
	IF_CLIENT_AND_VALID_OBJ_DO([&]() { g_factory->destroy_item_pickup(std::exchange(obj, nullptr)); });
}

void PickupNetObject::on_sync()
{
}

void PickupNetObject::on_spawn()
{
#ifdef JC_CLIENT
	obj = g_factory->spawn_general_item_pickup(get_position(), get_item_type(), get_lod());

	check(obj, "Could not create pickup");

	log(PURPLE, "{} {:x} spawned now {:x} at {:.2f} {:.2f} {:.2f}", typeid(*obj).name(), get_nid(), ptr(obj), get_position().x, get_position().y, get_position().z);
#endif
}

void PickupNetObject::on_despawn()
{
	destroy_object();
}

void PickupNetObject::on_net_var_change(NetObjectVarType var_type)
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

void PickupNetObject::serialize_derived_create(const Packet* p)
{
	p->add(get_item_type());
	p->add(get_lod());
}

void PickupNetObject::deserialize_derived_create(const Packet* p)
{
	set_item_type(p->get<decltype(item_type)>());
	set_lod(p->get_str());
}