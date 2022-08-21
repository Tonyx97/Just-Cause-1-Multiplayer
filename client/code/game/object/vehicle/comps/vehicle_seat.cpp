#include <defs/standard.h>

#include "vehicle_seat.h"

#include <mp/net.h>

#include <game/object/character/character.h>
#include <game/object/vehicle/vehicle.h>

namespace jc::vehicle_seat::hook
{
	DEFINE_HOOK_THISCALL(warp_character, 0x74DC30, void, VehicleSeat* seat, Character* character, CharacterHandleBase* unk, bool unk2)
	{
		/*if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
				if (const auto vehicle = seat->get_vehicle())
					if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
						g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, false, true);*/

		return warp_character_hook.call(seat, character, unk, unk2);
	}

	// the enter hook is in the interactable since the engine does it directly
	// using the interactable

	DEFINE_HOOK_THISCALL_S(leave, 0x76C010, void, VehicleSeat* seat)
	{
		const auto character = seat->get_character();

		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
				if (const auto vehicle = seat->get_vehicle())
					if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
						g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, false);

		return leave_hook.call(seat);
	}

	DEFINE_HOOK_THISCALL(instant_leave, 0x5AD280, void, Vehicle* vehicle, Character* character, bool is_local)
	{
		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
				if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
					g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, false);

		return instant_leave_hook.call(vehicle, character, is_local);
	}

	void apply()
	{
		warp_character_hook.hook();
		leave_hook.hook();
		instant_leave_hook.hook();
	}

	void undo()
	{
		instant_leave_hook.unhook();
		leave_hook.unhook();
		warp_character_hook.unhook();
	}
}

void VehicleSeat::warp_character(Character* character)
{
	//if (!get_character())
	{
		jc::vehicle_seat::hook::warp_character_hook.call(this, character, character->get_handle_base(), false);

		jc::this_call(0x5A1D40, character, true);
	}
}

void VehicleSeat::open_door(Character* character)
{
	jc::v_call(this, jc::vehicle_seat::vt::WARP_CHARACTER, character, character->get_handle_base(), true);
}

void VehicleSeat::kick_current()
{
	if (get_character())
		jc::vehicle_seat::hook::leave_hook.call(this);
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