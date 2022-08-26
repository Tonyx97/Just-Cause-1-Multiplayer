#include <defs/standard.h>

#include "vehicle.h"

#include <mp/net.h>

#include <core/keycode.h>

#include <game/sys/world/world.h>

#include <game/object/character/character.h>
#include <game/object/character/comps/vehicle_controller.h>
#include <game/object/weapon/weapon.h>

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

	float heli_c0 = 0.f,
		  heli_c1 = 0.f,
		  heli_c2 = 0.f,
		  heli_c3 = 0.f;
}

namespace jc::vehicle::hook
{
	template <typename VType, typename T>
	void land_vehicle_get_input(VType vehicle, CharacterController* controller, float* c0, float* c1, bool* braking, const T& hk)
	{
		if (!vehicle->is_alive())
			return;

		if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle)->cast<VehicleNetObject>())
		{
			const auto local_char = g_world->get_localplayer_character();

			const auto& info = vehicle_net->get_control_info();

			if (vehicle_net->is_owned())
			{
				// accept movement controls (forward and backward) only when the engine is turned on,
				// braking and wheels should be available even if the engine is turned off obviously

				if (vehicle->get_engine_state())
					*c0 = g_key->game_get_joystick_value(0x24) - g_key->game_get_joystick_value(0x25);
				else *c0 = 0.f;

				*c1 = g_key->game_get_joystick_value(0x27) - g_key->game_get_joystick_value(0x26);
				*braking = g_key->game_is_key_down(0x28);

				if (*c0 != info.c0 || *c1 != info.c1 || *braking != info.braking)
				{
					vehicle_net->set_control_info(*c0, *c1, 0.f, 0.f, *braking);

					g_net->send_reliable(PlayerPID_VehicleControl, vehicle_net, info.pack());
				}
			}
			else
			{
				*c0 = info.c0;
				*c1 = info.c1;
				*braking = info.braking;
			}
		}

		float dummy_x, dummy_y;
		bool dummy_braking;

		hk(vehicle, controller, &dummy_x, &dummy_y, &dummy_braking);
	}

	template <typename VType, typename T>
	void airplane_get_input(VType vehicle, CharacterController* controller, float* c0, float* c1, float* c2, float* c3, const T& hk)
	{
		if (!vehicle->is_alive())
			return;

		if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle)->cast<VehicleNetObject>())
		{
			const auto local_char = g_world->get_localplayer_character();

			const auto& info = vehicle_net->get_control_info();

			if (vehicle_net->is_owned())
			{
				*c1 = g_key->game_get_joystick_value(0x3A) - g_key->game_get_joystick_value(0x39);
				*c2 = g_key->game_get_joystick_value(0x37) - g_key->game_get_joystick_value(0x38);
				*c3 = 0.f;

				if (g_key->game_is_key_down(0x3D))
					*c0 = g_key->game_get_joystick_value(0x3D);

				if (g_key->game_is_key_down(0x3E))
					*c0 = -g_key->game_get_joystick_value(0x3E);

				if (*c0 != info.c0 || *c1 != info.c1 || *c2 != info.c2 || *c3 != info.c3)
				{
					vehicle_net->set_control_info(*c0, *c1, *c2, *c3);

					g_net->send_reliable(PlayerPID_VehicleControl, vehicle_net, info.pack());
				}
			}
			else
			{
				*c0 = info.c0;
				*c1 = info.c1;
				*c2 = info.c2;
				*c3 = info.c3;
			}
		}

		float dummy_x, dummy_y, dummy_forward, dummy_backward;

		hk(vehicle, controller, &dummy_x, &dummy_y, &dummy_forward, &dummy_backward);
	}

	DEFINE_HOOK_THISCALL(car_get_input, 0x82C990, void, Car* car, CharacterController* controller, float* c0, float* c1, bool* braking)
	{
		land_vehicle_get_input(car, controller, c0, c1, braking, car_get_input_hook.original);
	}

	DEFINE_HOOK_THISCALL(motorbike_get_input, 0x709E60, void, MotorBike* motorbike, CharacterController* controller, float* c0, float* c1, bool* braking)
	{
		land_vehicle_get_input(motorbike, controller, c0, c1, braking, motorbike_get_input_hook.original);
	}

	DEFINE_HOOK_THISCALL(airplane_get_input, 0x831570, void, AirPlane* airplane, CharacterController* controller, float* c0, float* c1, float* c2, float* c3)
	{
		airplane_get_input(airplane, controller, c0, c1, c2, c3, airplane_get_input_hook.original);
	}

	DEFINE_HOOK_THISCALL(helicopter_get_input, 0x82D310, void, Helicopter* helicopter, CharacterController* controller, float* c0, float* c1, float* c2, float* c3)
	{
		if (!helicopter->is_alive())
			return;

		if (const auto vehicle_net = g_net->get_net_object_by_game_object(helicopter)->cast<VehicleNetObject>())
		{
			heli_c0 = heli_c1 = heli_c2 = heli_c3 = 0.f;
			helicopter_input_dispatching = vehicle_net;
			helicopter_get_input_hook.call(helicopter, controller, c0, c1, c2, c3);
			helicopter_input_dispatching = nullptr;

			if (vehicle_net->is_owned() && vehicle_net->should_sync_this_tick())
			{
				const auto& info = vehicle_net->get_control_info();

				g_net->send_reliable(PlayerPID_VehicleControl, vehicle_net, info.pack());

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

	DEFINE_HOOK_THISCALL_S(land_vehicle_honk, 0x856060, void, LandVehicle* land_vehicle)
	{
		if (!land_vehicle->is_alive())
			return;

		if (const auto vehicle_net = g_net->get_net_object_by_game_object(land_vehicle)->cast<VehicleNetObject>())
			g_net->send_reliable(PlayerPID_VehicleHonk, vehicle_net);

		land_vehicle_honk_hook.call(land_vehicle);
	}

	DEFINE_HOOK_THISCALL_S(vehicle_fire, 0x636820, bool, Vehicle* vehicle)
	{
		if (!vehicle->is_alive())
			return false;

		std::vector<VehicleNetObject::FireInfo> fire_info;

		vehicle->for_each_current_weapon([&](int i, Weapon* weapon, uint32_t)
		{
			const auto muzzle = weapon->get_muzzle_transform()->get_position();
			const auto aim_target = weapon->get_aim_target();
			const auto direction = glm::normalize(aim_target - muzzle);

			fire_info.emplace_back(i, muzzle, direction);
		});

		const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle)->cast<VehicleNetObject>();

		if (vehicle_net)
			vehicle_net->set_fire_info(fire_info);
		else return vehicle_fire_hook.call(vehicle);

		const auto weapon_index = static_cast<uint8_t>(vehicle->get_current_weapon_index());
		const auto weapon_type = static_cast<uint8_t>(vehicle->get_current_weapon_type());
		const auto ok = vehicle_fire_hook.call(vehicle);

		if (ok)
			g_net->send_reliable<ChannelID_Generic>(PlayerPID_VehicleFire, vehicle_net, weapon_index, weapon_type, fire_info);

		return ok;
	}

	void apply()
	{
		car_get_input_hook.hook();
		motorbike_get_input_hook.hook();
		airplane_get_input_hook.hook();
		helicopter_get_input_hook.hook();
		//boat_get_input_hook.hook();
		land_vehicle_honk_hook.hook();
		vehicle_fire_hook.hook();
	}

	void undo()
	{
		vehicle_fire_hook.unhook();
		land_vehicle_honk_hook.unhook();
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
		case 0x2E: info.c2 = *value - heli_c2; break;		// forward
		case 0x2F: heli_c2 = *value; break;					// backward
		case 0x30: info.c3 = *value - heli_c3; break;		// left
		case 0x31: heli_c3 = *value; break;					// right
		case 0x32: info.c0 = *value - heli_c0; break;		// left siding
		case 0x33: heli_c0 = *value; break;					// right siding
		case 0x34:
		{
			// up

			heli_c1 = *value;

			if (heli_c1 > 0.f)
				info.c1 = heli_c1;

			break;
		}
		case 0x35:
		{
			// down

			if (heli_c1 <= 0.f)
				info.c1 = *value;

			break;
		}
		}

		vehicle_net->set_control_info(info);
	}
	else
	{
		switch (control)
		{
		case 0x2E: *value = info.c2; break;
		case 0x2F: *value = 0.f; break;
		case 0x30: *value = info.c3; break;
		case 0x31: *value = 0.f; break;
		case 0x32: *value = info.c0; break;
		case 0x33: *value = 0.f; break;
		case 0x34: *value = info.c1; break;
		case 0x35: *value = -info.c1; break;
		}
	}
}

void Vehicle::write_engine_state(bool v)
{
	jc::write(v, this, jc::vehicle::ENGINE_STATE);
}

void Vehicle::start_engine_sound(bool v)
{
	if (v)
		jc::v_call(this, jc::vehicle::vt::START_ENGINE_SOUND);
	else jc::v_call(this, jc::vehicle::vt::STOP_ENGINE_SOUND);
}

void Vehicle::set_velocity(const vec3& v)
{
	jc::v_call(this, jc::vehicle::vt::SET_VELOCITY, &v);
}

void Vehicle::honk()
{
	switch (get_type())
	{
	case VehicleTypeID_Car:
	case VehicleTypeID_MotorBike:
	{
		// todojc - wtf is this (clean this when we know exactly what it is, with names etc)

		const auto sound_comp = get_sound_component();

		auto res = (*(int(__thiscall**)(ptr, int, ptr))(**(ptr**)(sound_comp + 0xC0) + 0x30))(
			*(ptr*)(sound_comp + 0xC0),
			5,
			*(ptr*)(sound_comp + 4));

		if (res)
		{
			if ((*(int(__thiscall**)(int))(*(ptr*)res + 0x6C))(res))
			{
				ptr v3 = (*(ptr(__thiscall**)(int))(*(ptr*)res + 0x6C))(res);
				(*(void(__thiscall**)(int, int))(*(ptr*)v3 + 0x10))(v3, sound_comp + 0x50);
				(*(int(__thiscall**)(int))(*(ptr*)res + 0x18))(res);
			}
		}
	}
	}
}

void Vehicle::set_engine_state(bool v, bool sync)
{
	if (get_engine_state() == v)
		return;

	switch (get_type())
	{
	case VehicleTypeID_Car:
	case VehicleTypeID_MotorBike:
	case VehicleTypeID_Tank:
	case VehicleTypeID_Boat:
	case VehicleTypeID_Submarine:
	{
		write_engine_state(v);
		start_engine_sound(v);
		break;
	}
	}

	if (sync)
		if (const auto vehicle_net = g_net->get_net_object_by_game_object(this)->cast<VehicleNetObject>())
			g_net->send_reliable(PlayerPID_VehicleEngineState, vehicle_net, v);
}

void Vehicle::set_current_weapon_index(uint32_t v)
{
	jc::write(v, this, jc::vehicle::CURRENT_WEAPON_INDEX);
}

void Vehicle::set_current_weapon_type(uint32_t v)
{
	jc::write(v, this, jc::vehicle::CURRENT_WEAPON_TYPE);
}

void Vehicle::for_each_weapon(const vehicle_weapon_fn_t& fn)
{
	const auto weapons = jc::read<jc::stl::vector<VehicleWeapon*>>(this, jc::vehicle::WEAPONS);

	for (int i = 0; auto veh_weapon : weapons)
	{
		if (const auto weapon = veh_weapon->get_weapon())
			fn(i, weapon, veh_weapon->get_type());

		++i;
	}
}

void Vehicle::for_each_current_weapon(const vehicle_weapon_fn_t& fn)
{
	const auto current_weapon_type = get_current_weapon_type();

	for_each_weapon([&](int i, Weapon* weapon, uint32_t type)
	{
		if (type == current_weapon_type || i == current_weapon_type)
			fn(i, weapon, type);
	});
}

bool Vehicle::get_engine_state() const
{
	return jc::read<bool>(this, jc::vehicle::ENGINE_STATE);
}

ptr Vehicle::get_sound_component() const
{
	return jc::read<ptr>(this, jc::vehicle::SOUND_COMPONENT);
}

uint8_t Vehicle::get_type() const
{
	return static_cast<uint8_t>(jc::v_call<int>(this, jc::vehicle::vt::GET_TYPE));
}

uint32_t Vehicle::get_current_weapon_index() const
{
	return jc::read<uint32_t>(this, jc::vehicle::CURRENT_WEAPON_INDEX);
}

uint32_t Vehicle::get_current_weapon_type() const
{
	return jc::read<uint32_t>(this, jc::vehicle::CURRENT_WEAPON_TYPE);
}

vec3 Vehicle::get_velocity() const
{
	vec3 out;

	return *jc::v_call<vec3*>(this, jc::vehicle::vt::GET_VELOCITY, &out);
}

Weapon* Vehicle::get_weapon(int i) const
{
	const auto veh_weapon = jc::read<jc::stl::vector<VehicleWeapon*>>(this, jc::vehicle::WEAPONS)[i];
	return veh_weapon ? veh_weapon->get_weapon() : nullptr;
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