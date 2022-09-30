#include <defs/standard.h>

#include "vehicle_type.h"

#include "../game_resource/ee_resource.h"
#include "../vehicle/vehicle.h"

VehicleType* VehicleType::CREATE()
{
	auto vehicle_type = jc::c_call<VehicleType*>(jc::vehicle_type::fn::CREATE, 0x1F8, 0xA);

	jc::this_call(jc::vehicle_type::fn::SETUP, vehicle_type, true);

	return vehicle_type;
}

void VehicleType::load(const std::string& class_name, const std::string& ee_name, object_base_map* map)
{
	const jc::stl::string _class_name = class_name;
	const jc::stl::string _ee_name = ee_name;

	jc::this_call(jc::vehicle_type::fn::LOAD, this, &_class_name, &_ee_name, map);
}

bool VehicleType::is_loaded() const
{
	return jc::read<bool>(this, jc::vehicle_type::IS_LOADED);
}

ExportedEntityResource* VehicleType::get_ee_resource() const
{
	return jc::read<ExportedEntityResource*>(this, jc::vehicle_type::EE_RESOURCE);
}

ref<Vehicle> VehicleType::create_vehicle(const Transform& transform)
{
	ref<Vehicle> r;

	jc::this_call(jc::vehicle_type::fn::CREATE_VEHICLE, this, &r, &transform, false);

	return r;
}