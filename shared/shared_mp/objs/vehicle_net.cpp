#include <defs/standard.h>

#include <mp/net.h>

#include "vehicle_net.h"

#ifdef JC_CLIENT
#include <game/sys/core/factory_system.h>

#include <game/object/vehicle/vehicle.h>

VehicleNetObject::VehicleNetObject(NID nid, const TransformTR& transform)
{
	set_nid(nid);
	set_transform(transform);
	//set_transform_timer(50);
}

ObjectBase* VehicleNetObject::get_object_base()
{
	return get_object();
}
#else
VehicleNetObject::VehicleNetObject(SyncType sync_type, const TransformTR& transform)
{
	set_sync_type(sync_type);
	set_transform(transform);
}
#endif

VehicleNetObject::~VehicleNetObject()
{
#ifdef JC_CLIENT
	g_factory->destroy_vehicle(obj);
#endif
}

void VehicleNetObject::on_sync()
{
}

void VehicleNetObject::on_net_var_change(NetObjectVarType var_type)
{
#ifdef JC_CLIENT
	switch (var_type)
	{
	case NetObjectVar_Transform:
	case NetObjectVar_Position:
	case NetObjectVar_Rotation: obj->set_transform(Transform(get_position(), get_rotation())); break;
	//case NetObjectVar_Velocity: obj->get_physical()->set_velocity(get_velocity()); break;
	case NetObjectVar_Health: obj->set_hp(get_hp()); break;
	case NetObjectVar_MaxHealth: obj->set_max_hp(get_max_hp()); break;
}
#endif
}

void VehicleNetObject::set_info(float x, float y, bool braking)
{
	this->x = x;
	this->y = y;
	this->braking = braking;
}

bool VehicleNetObject::spawn()
{
	// if it's already spawned then do nothing

	if (is_spawned())
	{
		log(RED, "VehicleNetObject {:x} was already spawned, where are you calling this from?", get_nid());

		return false;
	}

#ifdef JC_CLIENT
	obj = g_factory->spawn_vehicle(54, get_position());

	check(obj, "Could not create vehicle");

	set_hp(obj->get_real_hp());
	set_max_hp(obj->get_max_hp());

	log(PURPLE, "{} {:x} spawned now {:x} at {:.2f} {:.2f} {:.2f}", typeid(*obj).name(), get_nid(), ptr(obj), get_position().x, get_position().y, get_position().z);
#endif

	set_spawned(true);

	return true;
}