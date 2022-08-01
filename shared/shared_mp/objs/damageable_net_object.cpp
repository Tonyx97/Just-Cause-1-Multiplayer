#include <defs/standard.h>

#include <mp/net.h>

#include "damageable_net_object.h"

#ifdef JC_CLIENT
#include <game/sys/factory_system.h>

#include <game/object/base/comps/physical.h>
#include <game/object/damageable_object/damageable_object.h>

DamageableNetObject::DamageableNetObject(NID nid, const TransformTR& transform)
{
	set_nid(nid);
	set_transform(transform);
}

void DamageableNetObject::on_sync()
{
}

ObjectBase* DamageableNetObject::get_object_base()
{
	return obj;
}
#else
DamageableNetObject::DamageableNetObject(const TransformTR& transform)
{
	set_sync_type(SyncType_Distance);
	set_transform(transform);
}
#endif

DamageableNetObject::~DamageableNetObject()
{
}

void DamageableNetObject::on_net_var_change(NetObjectVarType var_type)
{
#ifdef JC_CLIENT
	switch (var_type)
	{
	case NetObjectVar_Transform:
	case NetObjectVar_Position:
	case NetObjectVar_Rotation: obj->set_transform(Transform(get_position(), get_rotation())); break;
	case NetObjectVar_Velocity: obj->get_physical()->set_velocity(get_velocity()); break;
	case NetObjectVar_Health: obj->set_hp(get_hp()); break;
	case NetObjectVar_MaxHealth: obj->set_max_hp(get_max_hp()); break;
	}
#endif
}

bool DamageableNetObject::spawn()
{
	// if it's already spawned then do nothing

	if (is_spawned())
	{
		log(RED, "DamageableObject {:x} was already spawned, where are you calling this from?", get_nid());

		return false;
	}

#ifdef JC_CLIENT
	obj = g_factory->spawn_damageable_object(get_position(), "building_blocks\\general\\oil_barrel_red.lod", "models\\building_blocks\\general\\oil_barrel.pfx");

	check(obj, "Could not create damageable object");

	log(PURPLE, "DamageableObject {:x} spawned now {:x} at {:.2f} {:.2f} {:.2f}", get_nid(), ptr(obj), get_position().x, get_position().y, get_position().z);
#endif

	set_spawned(true);

	return true;
}