#include <defs/standard.h>

#include "vehicle.h"

#include <mp/net.h>

#include <game/object/character/character.h>
#include <game/object/character/comps/vehicle_controller.h>

#include <shared_mp/objs/vehicle_net.h>

namespace jc::vehicle::hook
{
	DEFINE_HOOK_THISCALL(car_get_joystick_input, 0x82C990, void, Vehicle* vehicle, void* player, float* y, float* x, bool* braking)
	{
		if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle)->cast<VehicleNetObject>())
		{
			const auto info = vehicle_net->get_info();

			const float _x = std::get<0>(info);
			const float _y = std::get<1>(info);
			const bool _braking = std::get<2>(info);

			if (vehicle_net->is_owned())
			{
				car_get_joystick_input_hook.call(vehicle, player, y, x, braking);

				if (*x != _x || *y != _y || *braking != _braking)
				{
					log(GREEN, "updating veh info");

					vehicle_net->set_info(*x, *y, *braking);

					g_net->send_reliable(PlayerPID_VehicleControl, vehicle_net, *x, *y, *braking);
				}
			}
			else
			{
				*x = _x;
				*y = _y;
				*braking = _braking;
			}
		}
	}

	void apply()
	{
		car_get_joystick_input_hook.hook();
	}

	void undo()
	{
		car_get_joystick_input_hook.unhook();
	}
}

void Vehicle::set_velocity(const vec3& v)
{
	jc::v_call(this, jc::vehicle::vt::SET_VELOCITY, &v);
}

vec3 Vehicle::get_velocity() const
{
	vec3 out;

	return *jc::v_call<vec3*>(this, jc::vehicle::vt::GET_VELOCITY, &out);
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