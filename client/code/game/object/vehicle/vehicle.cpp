#include <defs/standard.h>

#include "vehicle.h"

void Vehicle::set_velocity(const vec3& v)
{
	jc::v_call(this, jc::vehicle::vt::SET_VELOCITY, &v);
}

ref<VehicleSeat> Vehicle::get_driver_seat() const
{
    ref<VehicleSeat> r;

	jc::v_call(this, jc::vehicle::vt::GET_DRIVER_SEAT, &r);

	r.inc();

	return r;
}

ref<VehicleSeat> Vehicle::get_passenger_seat() const
{
	ref<VehicleSeat> r;

	jc::v_call(this, jc::vehicle::vt::GET_PASSENGER_SEAT, &r);

	r.inc();

	return r;
}

ref<VehicleSeat> Vehicle::get_special_seat() const
{
	ref<VehicleSeat> r;

	jc::v_call(this, jc::vehicle::vt::GET_SPECIAL_SEAT, &r);

	r.inc();

	return r;
}