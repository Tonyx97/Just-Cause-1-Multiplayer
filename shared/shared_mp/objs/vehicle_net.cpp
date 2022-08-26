#include <defs/standard.h>

#include <mp/net.h>

#include "vehicle_net.h"

#ifdef JC_CLIENT
#include <game/sys/core/factory_system.h>

#include <game/object/base/comps/physical.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/weapon/weapon.h>

VehicleNetObject::VehicleNetObject(NID nid, const TransformTR& transform)
{
	set_nid(nid);
	set_transform(transform);
	//set_transform_timer(5000);
	set_velocity_timer(500);
}

ObjectBase* VehicleNetObject::get_object_base()
{
	return get_object();
}

void VehicleNetObject::fire()
{
	for (const auto& info : fire_info)
		if (const auto weapon = obj->get_weapon(info.index))
		{
			weapon->set_last_shot_time(jc::nums::MAXF);
			weapon->force_fire();
			weapon->set_enabled(true);
			weapon->update();
		}
}

const VehicleNetObject::FireInfo* VehicleNetObject::get_fire_info_from_weapon(Weapon* weapon) const
{
	for (const auto& info : fire_info)
		if (obj->get_weapon(info.index) == weapon)
			return &info;

	return nullptr;
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
	case NetObjectVar_Velocity: obj->get_physical()->set_velocity(get_velocity()); break;
	case NetObjectVar_Health: obj->set_hp(get_hp()); break;
	case NetObjectVar_MaxHealth: obj->set_max_hp(get_max_hp()); break;
}
#endif
}

void VehicleNetObject::set_control_info(float c0, float c1, float c2, float c3, bool braking)
{
#ifdef JC_CLIENT
	if (!sync_this_tick)
		sync_this_tick = (
			c0 != control_info.c0 ||
			c1 != control_info.c1 ||
			c2 != control_info.c2 ||
			c3 != control_info.c3 ||
			braking != control_info.braking);
#endif

	control_info.c0 = c0;
	control_info.c1 = c1;
	control_info.c2 = c2;
	control_info.c3 = c3;
	control_info.braking = braking;
}

void VehicleNetObject::set_control_info(const ControlInfo& v)
{
	set_control_info(v.c0, v.c1, v.c2, v.c3, v.braking);
}

void VehicleNetObject::set_weapon_info(uint32_t index, uint32_t type)
{
	weapon_index = index;
	weapon_type = type;

#ifdef JC_CLIENT
	obj->set_current_weapon_index(index);
	obj->set_current_weapon_type(type);
#endif
}

void VehicleNetObject::set_fire_info(const std::vector<FireInfo>& v)
{
	fire_info = v;
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
	obj = g_factory->spawn_vehicle(get_object_id(), get_position());

	check(obj, "Could not create vehicle");

	log(PURPLE, "{} {:x} spawned now {:x} at {:.2f} {:.2f} {:.2f}", typeid(*obj).name(), get_nid(), ptr(obj), get_position().x, get_position().y, get_position().z);
#endif

	set_spawned(true);

	return true;
}