#pragma once

#include "land_vehicle.h"

namespace jc::car
{
	namespace vt
	{
	}
}

class Car : public LandVehicle
{
public:

	IMPL_OBJECT_TYPE_ID("CCar");
};