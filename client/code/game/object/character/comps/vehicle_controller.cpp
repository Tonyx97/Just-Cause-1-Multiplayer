#include <defs/standard.h>

#include "vehicle_controller.h"

Transform* VehicleController::get_transform()
{
	return REF(Transform*, this, jc::vehicle_controller::TRANSFORM);
}

void* VehicleController::get_vehicle()
{
	return jc::read<void*>(this, jc::vehicle_controller::VEHICLE);
}