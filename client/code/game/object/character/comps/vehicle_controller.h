#pragma once

namespace jc::vehicle_controller
{
	static constexpr uint32_t TRANSFORM = 0x4; // Transform
	static constexpr uint32_t VEHICLE	= 0x4C; // Vehicle*
}

class VehicleController
{
public:
	Transform* get_transform();

	void* get_vehicle();
};