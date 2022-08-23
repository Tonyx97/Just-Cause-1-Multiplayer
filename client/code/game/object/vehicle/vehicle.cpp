#include <defs/standard.h>

#include "vehicle.h"

#include <mp/net.h>

#include <core/keycode.h>

#include <game/sys/world/world.h>

#include <game/object/character/character.h>
#include <game/object/character/comps/vehicle_controller.h>

#include <shared_mp/objs/vehicle_net.h>

#include "car.h"
#include "motorbike.h"
#include "airplane.h"
#include "helicopter.h"
#include "boat.h"
#include "submarine.h"

namespace jc::vehicle
{
	VehicleNetObject* helicopter_input_dispatching = nullptr;

	float heli_x = 0.f,
		  heli_y = 0.f,
		  heli_forward = 0.f,
		  heli_backward = 0.f;
}

namespace jc::vehicle::hook
{
	template <typename VType, typename T>
	void land_vehicle_get_input(VType vehicle, CharacterController* controller, float* y, float* x, bool* braking, const T& hk)
	{
		if (!vehicle->is_alive())
			return;

		if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle)->cast<VehicleNetObject>())
		{
			const auto local_char = g_world->get_localplayer_character();

			const auto& info = vehicle_net->get_control_info();

			if (vehicle_net->is_owned())
			{
				*y = g_key->game_get_joystick_value(0x24) - g_key->game_get_joystick_value(0x25);
				*x = g_key->game_get_joystick_value(0x27) - g_key->game_get_joystick_value(0x26);

				*braking = g_key->game_is_key_down(0x28);

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

		float dummy_x, dummy_y;
		bool dummy_braking;

		hk(vehicle, controller, &dummy_x, &dummy_y, &dummy_braking);
	}

	template <typename VType, typename T>
	void airplane_get_input(VType vehicle, CharacterController* controller, float* y, float* x, float* forward, float* backward, const T& hk)
	{
		if (!vehicle->is_alive())
			return;

		if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle)->cast<VehicleNetObject>())
		{
			const auto local_char = g_world->get_localplayer_character();

			const auto& info = vehicle_net->get_control_info();

			if (vehicle_net->is_owned())
			{
				*backward = 0.f;
				*forward = g_key->game_get_joystick_value(0x37) - g_key->game_get_joystick_value(0x38);
				*x = g_key->game_get_joystick_value(0x3A) - g_key->game_get_joystick_value(0x39);

				if (g_key->game_is_key_down(0x3D))
					*y = g_key->game_get_joystick_value(0x3D);

				if (g_key->game_is_key_down(0x3E))
					*y = -g_key->game_get_joystick_value(0x3E);

				if (*x != info.x || *y != info.y || *forward != info.forward || *backward != info.backward)
				{
					vehicle_net->set_control_info(*x, *y, *forward, *backward);

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

		float dummy_x, dummy_y, dummy_forward, dummy_backward;

		hk(vehicle, controller, &dummy_x, &dummy_y, &dummy_forward, &dummy_backward);
	}

	DEFINE_HOOK_THISCALL(car_get_input, 0x82C990, void, Car* car, CharacterController* controller, float* y, float* x, bool* braking)
	{
		land_vehicle_get_input(car, controller, y, x, braking, car_get_input_hook.original);
	}

	DEFINE_HOOK_THISCALL(motorbike_get_input, 0x709E60, void, MotorBike* motorbike, CharacterController* controller, float* y, float* x, bool* braking)
	{
		land_vehicle_get_input(motorbike, controller, y, x, braking, motorbike_get_input_hook.original);
	}

	DEFINE_HOOK_THISCALL(airplane_get_input, 0x831570, void, AirPlane* airplane, CharacterController* controller, float* y, float* x, float* forward, float* backward)
	{
		airplane_get_input(airplane, controller, y, x, forward, backward, airplane_get_input_hook.original);
	}

	DEFINE_HOOK_THISCALL(helicopter_get_input, 0x82D310, void, Helicopter* helicopter, CharacterController* controller, float* y, float* x, float* forward, float* backward)
	{
		if (!helicopter->is_alive())
			return;

		if (const auto vehicle_net = g_net->get_net_object_by_game_object(helicopter)->cast<VehicleNetObject>())
		{
			heli_x = heli_y = heli_forward = heli_backward = 0.f;
			helicopter_input_dispatching = vehicle_net;
			helicopter_get_input_hook.call(helicopter, controller, y, x, forward, backward);
			helicopter_input_dispatching = nullptr;

			if (vehicle_net->is_owned() && vehicle_net->should_sync_this_tick())
			{
				const auto& info = vehicle_net->get_control_info();

				g_net->send_reliable(PlayerPID_VehicleControl, vehicle_net, info.x, info.y, info.forward, info.backward, false);

				vehicle_net->reset_sync();
			}
		}
	}

	DEFINE_HOOK_THISCALL(boat_get_input, 0x8326D0, void, Boat* boat, CharacterController* controller, float* y, float* x)
	{
		if (!boat->is_alive())
			return;

		// todojc - stupid boats won't work with players for some reasons

		//boat_get_input_hook.original(boat, player, y, x);
		//sea_vehicle_get_input(boat, player, y, x, boat_get_input_hook.original);
		//*y = 1.f;
	}

	void apply()
	{
		car_get_input_hook.hook();
		motorbike_get_input_hook.hook();
		airplane_get_input_hook.hook();
		helicopter_get_input_hook.hook();
		//boat_get_input_hook.hook();
	}

	void undo()
	{
		//boat_get_input_hook.unhook();
		helicopter_get_input_hook.unhook();
		airplane_get_input_hook.unhook();
		motorbike_get_input_hook.unhook();
		car_get_input_hook.unhook();
	}
}

bool jc::vehicle::getting_helicopter_input()
{
	return !!helicopter_input_dispatching;
}

void jc::vehicle::dispatch_helicopter_input(int control, float* value)
{
	const auto vehicle_net = helicopter_input_dispatching;
	const auto vehicle = vehicle_net->get_object();
	const auto local_char = g_world->get_localplayer_character();

	auto info = vehicle_net->get_control_info();

	if (vehicle_net->is_owned())
	{
		switch (control)
		{
		case 0x2E: info.forward = *value - heli_forward; break;		// forward
		case 0x2F: heli_forward = *value; break;					// backward
		case 0x30: info.backward = *value - heli_backward; break;	// left
		case 0x31: heli_backward = *value; break;					// right
		case 0x32: info.x = *value - heli_x; break;					// left siding
		case 0x33: heli_x = *value; break;							// right siding
		case 0x34:
		{
			// up

			heli_y = *value;

			if (heli_y > 0.f)
				info.y = heli_y;

			break;
		}
		case 0x35:
		{
			// down

			if (heli_y <= 0.f)
				info.y = *value;

			break;
		}
		}

		vehicle_net->set_control_info(info);
	}
	else
	{
		switch (control)
		{
		case 0x2E: *value = info.forward; break;
		case 0x2F: *value = 0.f; break;
		case 0x30: *value = info.backward; break;
		case 0x31: *value = 0.f; break;
		case 0x32: *value = info.x; break;
		case 0x33: *value = 0.f; break;
		case 0x34: *value = info.y; break;
		case 0x35: *value = -info.y; break;
		}
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