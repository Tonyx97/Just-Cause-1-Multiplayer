#include <defs/standard.h>

#include <game/transform/transform.h>
#include <game/sys/game/game_control.h>

#include "vehicle_spawn_point.h"

void VehicleSpawnPoint::set_vehicle_type(VehicleType* vehicle_type)
{
	jc::write(vehicle_type, this, jc::vehicle_spawn_point::TYPE);
}

std::string VehicleSpawnPoint::get_name()
{
	return jc::read<std::string>(this, jc::spawn_point::SPAWN_NAME);
}

std::string VehicleSpawnPoint::get_preset()
{
	return jc::read<std::string>(this, jc::spawn_point::PRESET_NAME);
}