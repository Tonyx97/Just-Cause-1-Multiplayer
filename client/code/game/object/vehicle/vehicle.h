#pragma once

#include "../action_point_owner/action_point_owner.h"

#include "comps/vehicle_seat.h"

namespace jc::vehicle
{
	static constexpr uint32_t DRIVER_SEAT			= 0xA0;
	static constexpr uint32_t PASSENGER_SEAT		= 0xA4;
	static constexpr uint32_t SPECIAL_SEAT			= 0xAC;

	namespace vt
	{
		static constexpr uint32_t GET_VELOCITY			= 48;
		static constexpr uint32_t SET_VELOCITY			= 50;
		static constexpr uint32_t GET_DRIVER_SEAT		= 53;
		static constexpr uint32_t GET_PASSENGER_SEAT	= 54;
		static constexpr uint32_t GET_SPECIAL_SEAT		= 55;
	}
}

enum VehicleFaction
{
	VehFaction_None,
	VehFaction_Agency = 2,
	VehFaction_Military,
	VehFaction_Police,
	VehFaction_Guerrilla,
	VehFaction_BlackHand,
	VehFaction_Montano,
	VehFaction_Rioja
};

class Vehicle : public ActionPointOwner
{
public:

	IMPL_OBJECT_TYPE_ID("CVehicle");

	void set_velocity(const vec3& v);

	vec3 get_velocity() const;

	ref<VehicleSeat> get_driver_seat() const;
	ref<VehicleSeat> get_passenger_seat() const;
	ref<VehicleSeat> get_special_seat() const;
};