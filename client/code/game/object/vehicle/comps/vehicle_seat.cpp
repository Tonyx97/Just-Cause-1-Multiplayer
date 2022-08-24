#include <defs/standard.h>

#include "vehicle_seat.h"

#include <mp/net.h>

#include <game/object/character/comps/stance_controller.h>
#include <game/object/character/character.h>
#include <game/object/vehicle/vehicle.h>

namespace jc::vehicle_seat::hook
{
	DEFINE_HOOK_THISCALL(driver_seat_enter, 0x76A3A0, void, VehicleSeat* seat, Character* character, CharacterController* unk, bool unk2)
	{
		/*if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
				if (const auto vehicle = seat->get_vehicle())
					if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
						if (unk2)
							g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, VehicleEnterExit_OpenDoor);*/

		driver_seat_enter_hook.call(seat, character, unk, unk2);
	}

	DEFINE_HOOK_THISCALL(warp_character, 0x74DC30, void, VehicleSeat* seat, Character* character, CharacterController* unk, bool unk2)
	{
		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
				if (const auto vehicle = seat->get_vehicle())
					if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
						g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, VehicleEnterExit_Enter);

		return warp_character_hook.call(seat, character, unk, unk2);
	}

	DEFINE_HOOK_THISCALL_S(leave, 0x76C010, void, VehicleSeat* seat)
	{
		const auto some_f = jc::read<float>(seat, 0x1A4);

		if (some_f < -3.f)
		{
			const auto character = seat->get_character();

			if (const auto localplayer = g_net->get_localplayer())
				if (const auto local_char = localplayer->get_character(); character == local_char)
					if (const auto vehicle = seat->get_vehicle())
						if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
							g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, VehicleEnterExit_Exit, false);

			leave_hook.call(seat);
		}
	}

	DEFINE_HOOK_THISCALL(instant_leave, 0x5AD280, void, Vehicle* vehicle, Character* character, bool is_local)
	{
		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
				if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
					g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, VehicleEnterExit_Exit, true);

		return instant_leave_hook.call(vehicle, character, is_local);
	}

	void apply()
	{
		driver_seat_enter_hook.hook();
		warp_character_hook.hook();
		leave_hook.hook();
		instant_leave_hook.hook();
	}

	void undo()
	{
		instant_leave_hook.unhook();
		leave_hook.unhook();
		warp_character_hook.unhook();
		driver_seat_enter_hook.unhook();
	}
}

void VehicleSeat::warp_character(Character* character, bool warp)	// 76578B
{
	const auto current_character = get_character();

	if (current_character == character || current_character)
		return;

	jc::vehicle_seat::hook::driver_seat_enter_hook.call(this, character, character->get_controller(), false);

	if (warp)
		character->set_enter_vehicle_stance(true);
}

void VehicleSeat::open_door(Character* character)
{
	const auto current_character = get_character();

	if (current_character == character || current_character)
		return;

	jc::vehicle_seat::hook::driver_seat_enter_hook.call(this, character, character->get_controller(), true);
}

void VehicleSeat::kick_current(bool instant)
{
	if (!get_character())
		return;

	if (instant)
		jc::vehicle_seat::hook::instant_leave_hook.call(get_vehicle(), get_character(), false);
	else
	{
		const auto some_f = jc::read<float>(this, 0x1A4);

		if (some_f < -3.f)
			jc::vehicle_seat::hook::leave_hook.call(this);
	}
}

uint8_t VehicleSeat::get_type() const
{
	return static_cast<uint8_t>(jc::v_call<int>(this, jc::vehicle_seat::vt::GET_TYPE));
}

Vehicle* VehicleSeat::get_vehicle() const
{
	return jc::read<Vehicle*>(this, jc::vehicle_seat::VEHICLE);
}

Character* VehicleSeat::get_character() const
{
	return jc::read<Character*>(this, jc::vehicle_seat::CHARACTER);
}

Interactable* VehicleSeat::get_interactable() const
{
	return jc::read<Interactable*>(this, jc::vehicle_seat::INTERACTABLE);
}