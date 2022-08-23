#pragma once

#include "../action_point_owner/action_point_owner.h"

#include "comps/vehicle_seat.h"

class Vehicle;

namespace jc::vehicle
{
	static constexpr uint32_t DRIVER_SEAT			= 0xA0;
	static constexpr uint32_t PASSENGER_SEAT		= 0xA4;
	static constexpr uint32_t ROOFTOP_SEAT			= 0xA8;
	static constexpr uint32_t SPECIAL_SEAT			= 0xAC;
	static constexpr uint32_t ENGINE_STATE			= 0x231;
	static constexpr uint32_t SOUND_COMPONENT		= 0x404;

	namespace vt
	{
		static constexpr uint32_t GET_TYPE				= 37;
		static constexpr uint32_t GET_VELOCITY			= 48;
		static constexpr uint32_t SET_VELOCITY			= 50;
		static constexpr uint32_t GET_DRIVER_SEAT		= 53;
		static constexpr uint32_t GET_PASSENGER_SEAT	= 54;
		static constexpr uint32_t GET_SPECIAL_SEAT		= 55;
		static constexpr uint32_t HONK					= 76;
		static constexpr uint32_t START_ENGINE_SOUND	= 82;
		static constexpr uint32_t STOP_ENGINE_SOUND		= 83;
	}

	namespace hook
	{
		void apply();
		void undo();
	}

	bool getting_helicopter_input();

	void dispatch_helicopter_input(int control, float* value);
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

enum VehicleTypeID : uint8_t
{
	VehicleTypeID_None,
	VehicleTypeID_Car,
	VehicleTypeID_MotorBike,
	VehicleTypeID_AirPlane,
	VehicleTypeID_Helicopter,
	VehicleTypeID_Boat,
	VehicleTypeID_Tank,
	VehicleTypeID_MountedGun,
	VehicleTypeID_Submarine,
};

class Vehicle : public ActionPointOwner
{
private:

	void write_engine_state(bool v);
	void start_engine_sound(bool v);

public:

	IMPL_OBJECT_TYPE_ID("CVehicle");

	void set_velocity(const vec3& v);
	void honk();
	void set_engine_state(bool v, bool sync = true);

	bool get_engine_state() const;

	ptr get_sound_component() const;

	uint8_t get_type() const;

	vec3 get_velocity() const;

	ref<VehicleSeat> get_driver_seat() const;
	ref<VehicleSeat> get_passenger_seat() const;
	ref<VehicleSeat> get_special_seat() const;
};