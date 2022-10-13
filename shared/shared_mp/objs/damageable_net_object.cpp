#include <defs/standard.h>

#include <mp/net.h>

#include "damageable_net_object.h"

#ifdef JC_CLIENT
#include <game/sys/core/factory_system.h>

#include <game/object/base/comps/physical.h>
#include <game/object/damageable_object/damageable_object.h>

DamageableNetObject::DamageableNetObject(NID nid, const TransformTR& transform)
{
	set_nid(nid);
	set_transform(transform);
}

ObjectBase* DamageableNetObject::get_object_base() const
{
	return obj;
}
#else
DamageableNetObject::DamageableNetObject(SyncType sync_type, const TransformTR& transform)
{
	set_sync_type(sync_type);
	set_transform(transform);
}
#endif

DamageableNetObject::~DamageableNetObject()
{
	destroy_object();
}

void DamageableNetObject::destroy_object()
{
#ifdef JC_CLIENT
	if (obj)
		g_factory->destroy_damageable_object(std::exchange(obj, nullptr));
#endif
}

void DamageableNetObject::on_spawn()
{
#ifdef JC_CLIENT
	obj = g_factory->spawn_damageable_object(get_position(), get_object_id(), get_pfx_id());

	check(obj, "Could not create damageable object");

	log(PURPLE, "{} {:x} spawned now {:x} at {:.2f} {:.2f} {:.2f}", typeid(*obj).name(), get_nid(), ptr(obj), get_position().x, get_position().y, get_position().z);
#endif
}

void DamageableNetObject::on_despawn()
{
	destroy_object();
}

void DamageableNetObject::on_sync()
{
}

void DamageableNetObject::on_net_var_change(NetObjectVarType var_type)
{
#ifdef JC_CLIENT
	switch (var_type)
	{
	case NetObjectVar_Transform:
	case NetObjectVar_Position:
	case NetObjectVar_Rotation: /*obj->set_transform(Transform(get_position(), get_rotation()));*/ break;
	case NetObjectVar_Velocity: obj->get_physical()->set_velocity(get_velocity()); break;
	case NetObjectVar_Health: obj->set_hp(get_hp()); break;
	case NetObjectVar_MaxHealth: obj->set_max_hp(get_max_hp()); break;
	}
#endif
}