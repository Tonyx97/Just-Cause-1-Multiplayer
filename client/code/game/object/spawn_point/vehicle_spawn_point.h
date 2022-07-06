#pragma once

#include "object_spawn_point.h"

namespace jc::vehicle_spawn_point
{
	static constexpr uint32_t TYPE = 0xE4; // VehicleType*
}

class VehicleType
{
public:
};

class VehicleSpawnPoint : public ObjectSpawnPoint
{
public:

	IMPL_OBJECT_TYPE_ID("CVehicleSpawnPoint");

	void set_vehicle_type(VehicleType* vehicle_type);

	std::string get_name();
	std::string get_preset();
};