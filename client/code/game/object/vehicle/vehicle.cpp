#include <defs/standard.h>

#include "vehicle.h"

#include <mp/net.h>

#include <game/object/character/character.h>
#include <game/object/character/comps/vehicle_controller.h>

#include <shared_mp/objs/vehicle_net.h>

#include "car.h"
#include "motorbike.h"
#include "boat.h"
#include "submarine.h"
#include "airplane.h"
#include "helicopter.h"

namespace jc::vehicle::hook
{
	template <typename VType, typename T>
	void land_vehicle_get_input(VType vehicle, void* player, float* y, float* x, bool* braking, const T& hk)
	{
		if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle)->cast<VehicleNetObject>())
		{
			const auto& info = vehicle_net->get_control_info();

			if (vehicle_net->is_owned())
			{
				hk(vehicle, player, y, x, braking);

				if (*x != info.x || *y != info.y || *braking != info.braking)
				{
					vehicle_net->set_control_info(*x, *y, 0.f, 0.f, *braking);

					g_net->send_reliable(PlayerPID_VehicleControl, vehicle_net, *x, *y, 0.f, 0.f, *braking);
				}
			}
			else
			{
				*x = info.x;
				*y = info.y;
				*braking = info.braking;
			}
		}
	}

	template <typename VType, typename T>
	void sea_vehicle_get_input(VType vehicle, void* player, float* y, float* x, const T& hk)
	{
		if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle)->cast<VehicleNetObject>())
		{
			const auto& info = vehicle_net->get_control_info();

			if (vehicle_net->is_owned())
			{
				hk(vehicle, player, y, x);

				if (*x != info.x || *y != info.y)
				{
					vehicle_net->set_control_info(*x, *y, 0.f, 0.f, false);

					g_net->send_reliable(PlayerPID_VehicleControl, vehicle_net, *x, *y, 0.f, 0.f, false);
				}
			}
			else
			{
				*x = info.x;
				*y = info.y;
			}
		}
	}

	template <typename VType, typename T>
	void air_vehicle_get_input(VType vehicle, void* player, float* y, float* x, float* forward, float* backward, const T& hk)
	{
		if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle)->cast<VehicleNetObject>())
		{
			const auto& info = vehicle_net->get_control_info();

			if (vehicle_net->is_owned())
			{
				hk(vehicle, player, y, x, forward, backward);

				if (*x != info.x || *y != info.y || *forward != info.forward || *backward != info.backward)
				{
					vehicle_net->set_control_info(*x, *y, *forward, *backward, false);

					g_net->send_reliable(PlayerPID_VehicleControl, vehicle_net, *x, *y, *forward, *backward, false);
				}
			}
			else
			{
				*x = info.x;
				*y = info.y;
				*forward = info.forward;
				*backward = info.backward;
			}
		}
	}

	DEFINE_HOOK_THISCALL(car_get_input, 0x82C990, void, Car* car, void* player, float* y, float* x, bool* braking)
	{
		//land_vehicle_get_input(car, player, y, x, braking, car_get_input_hook.original);
	}

	DEFINE_HOOK_THISCALL(motorbike_get_input, 0x709E60, void, MotorBike* motorbike, void* player, float* y, float* x, bool* braking)
	{
		land_vehicle_get_input(motorbike, player, y, x, braking, motorbike_get_input_hook.original);
	}

	DEFINE_HOOK_THISCALL(boat_get_input, 0x8326D0, void, Boat* boat, void* player, float* y, float* x)
	{
		// todojc - stupid boats won't work with players for some reasons
		
		//boat_get_input_hook.original(boat, player, y, x);
		//sea_vehicle_get_input(boat, player, y, x, boat_get_input_hook.original);
		//*y = 1.f;
	}

	DEFINE_HOOK_THISCALL(airplane_get_input, 0x831570, void, AirPlane* airplane, void* player, float* y, float* x, float* forward, float* backward)
	{
		air_vehicle_get_input(airplane, player, y, x, forward, backward, airplane_get_input_hook.original);
	}

	DEFINE_HOOK_THISCALL(helicopter_get_input, 0x82D310, void, Helicopter* helicopter, void* player, float* y, float* x, float* forward, float* backward)
	{
		air_vehicle_get_input(helicopter, player, y, x, forward, backward, helicopter_get_input_hook.original);

		log(RED, "{} {} {} {}", *y, *x, *forward, *backward);
	}

	void apply()
	{
		//car_get_input_hook.hook();
		//motorbike_get_input_hook.hook();
		//boat_get_input_hook.hook();
		//airplane_get_input_hook.hook();
		//helicopter_get_input_hook.hook();
	}

	void undo()
	{
		//helicopter_get_input_hook.unhook();
		//airplane_get_input_hook.unhook();
		//boat_get_input_hook.unhook();
		//motorbike_get_input_hook.unhook();
		//car_get_input_hook.unhook();
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