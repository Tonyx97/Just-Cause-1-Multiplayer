#include <defs/standard.h>

#include "vehicle_seat.h"

#include <mp/net.h>

#include <game/object/character/comps/stance_controller.h>
#include <game/object/character/character.h>
#include <game/object/vehicle/vehicle.h>

namespace jc::vehicle_seat::hook
{
	DEFINE_HOOK_THISCALL(warp_character, 0x74DC30, void, VehicleSeat* seat, Character* character, CharacterController* unk, bool unk2)
	{
		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
				if (const auto vehicle = seat->get_vehicle())
					if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
					{
						const auto char_vehicle = character->get_vehicle();

						localplayer->set_vehicle(vehicle_net->cast<VehicleNetObject>());

						g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, seat->get_type(), VehicleEnterExit_Enter);
					}

		return warp_character_hook(seat, character, unk, unk2);
	}

	template <typename T>
	void vehicle_seat_enter(VehicleSeat* seat, Character* character, CharacterController* controller, bool unk2, const T& hk)
	{
		// the engine forces the controller to be the player one, causing insane
		// issues because other players can make other exist the vehicle in their
		// remote game, we will get the actual controller of the player so it works properly
		// if the character is an actual player

		if (const auto player = g_net->get_player_by_character(character))
			controller = character->get_controller();

		hk(seat, character, controller, unk2);
	}

	DEFINE_HOOK_THISCALL(driver_seat_dispatch_entry, 0x76A3A0, void, VehicleSeat* seat, Character* character, CharacterController* controller, bool unk2)
	{
		vehicle_seat_enter(seat, character, controller, unk2, driver_seat_dispatch_entry_hook.original);
	}

	DEFINE_HOOK_THISCALL(passenger_seat_dispatch_entry, 0x785760, void, VehicleSeat* seat, Character* character, CharacterController* controller, bool unk2)
	{
		vehicle_seat_enter(seat, character, controller, unk2, passenger_seat_dispatch_entry_hook.original);
	}

	DEFINE_HOOK_THISCALL(special_seat_dispatch_entry, 0x642FF0, void, VehicleSeat* seat, Character* character, CharacterController* controller, bool unk2)
	{
		vehicle_seat_enter(seat, character, controller, unk2, special_seat_dispatch_entry_hook.original);
	}

	DEFINE_HOOK_THISCALL(roof_seat_dispatch_entry, 0x8B1F00, void, VehicleSeat* seat, Character* character, CharacterController* controller, bool unk2)
	{
		vehicle_seat_enter(seat, character, controller, unk2, roof_seat_dispatch_entry_hook.original);
	}

	DEFINE_INLINE_HOOK_IMPL(driver_to_roof_seat, 0x76BECD)
	{
		const auto driver_seat = ihp->read_ebp<VehicleSeat*>(0x80);

		if (const auto lp = g_net->get_localplayer())
			if (const auto driver_char = driver_seat->get_character(); driver_char == lp->get_character())
				if (const auto vehicle_net = lp->get_vehicle())
					g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, driver_seat->get_type(), VehicleEnterExit_DriverToRoof);
	}

	DEFINE_INLINE_HOOK_IMPL(passenger_to_driver_seat, 0x78680B)
	{
		const auto passenger_seat = ihp->read_ebp<VehicleSeat*>(0x1CC);

		if (const auto lp = g_net->get_localplayer())
			if (const auto passenger_char = passenger_seat->get_character(); passenger_char == lp->get_character())
				if (const auto vehicle_net = lp->get_vehicle())
				{
					auto driver_char = ihp->read_ebp<Character*>(0x24);

					if (!driver_char || !(driver_char->get_flags() & (1 << 10)))	// todore - not sure what's this flag but the engine does this
						driver_char = nullptr;

					const auto driver_player = g_net->get_player_by_character(driver_char); // could be null if there is no driver

					if (passenger_seat->is_occupied())
						g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, passenger_seat->get_type(), VehicleEnterExit_PassengerToDriver, driver_player);
				}
	}

	DEFINE_HOOK_THISCALL(instant_exit, 0x5AD280, void, Vehicle* vehicle, Character* character, bool is_local)
	{
		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
				if (const auto seat = local_char->get_vehicle_seat())
					if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
					{
						localplayer->set_vehicle(nullptr);

						g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, seat->get_type(), VehicleEnterExit_Exit, false);
					}

		return instant_exit_hook(vehicle, character, is_local);
	}

	DEFINE_INLINE_HOOK_IMPL(driver_seat_exit, 0x76C2E5)
	{
		const auto driver_seat = ihp->read_ebp<VehicleSeat*>(0x120);

		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); driver_seat->get_character() == local_char)
				if (const auto vehicle = driver_seat->get_vehicle())
					if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
					{
						const auto jump_out = ihp->read_ebp<bool>(0xE);

						localplayer->set_vehicle(nullptr);

						g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, driver_seat->get_type(), VehicleEnterExit_Exit, jump_out);
					}
	}

	DEFINE_INLINE_HOOK_IMPL(passenger_seat_exit, 0x786448)
	{
		const auto passenger_seat = ihp->read_ebp<VehicleSeat*>(0x1CC);

		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); passenger_seat->get_character() == local_char)
				if (const auto vehicle = passenger_seat->get_vehicle())
					if (const auto vehicle_net = g_net->get_net_object_by_game_object(vehicle))
					{
						localplayer->set_vehicle(nullptr);

						g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, passenger_seat->get_type(), VehicleEnterExit_Exit, false);
					}
	}

	void enable(bool apply)
	{
		// generic

		warp_character_hook.hook(apply);

		// specific seats dispatch entry hooks

		driver_seat_dispatch_entry_hook.hook(apply);
		passenger_seat_dispatch_entry_hook.hook(apply);
		special_seat_dispatch_entry_hook.hook(apply);
		roof_seat_dispatch_entry_hook.hook(apply);

		// hooks to sync and handle movements from seat to seat

		driver_to_roof_seat_hook.hook(apply);
		passenger_to_driver_seat_hook.hook(apply);

		// hooks to sync seat exits

		instant_exit_hook.hook(apply);
		driver_seat_exit_hook.hook(apply);
		passenger_seat_exit_hook.hook(apply);
	}
}

void VehicleSeat::warp_character(Character* character, bool warp)	// 76578B
{
	const auto current_character = get_character();

	if (current_character == character || current_character)
		return;

	dispatch_entry(character, false);

	if (warp)
		character->set_stance_enter_vehicle_right(true);
}

void VehicleSeat::open_door(Character* character)
{
	const auto current_character = get_character();

	if (current_character == character || current_character)
		return;

	dispatch_entry(character, true);
}

void VehicleSeat::exit(bool jump_out)
{
	if (const auto character = get_character())
		if (const auto vehicle = get_vehicle())
		{
			switch (get_type())
			{
			case VehicleSeat_Driver:
			{
				// jump out bool is used only in the driver seat.
				// the passenger seat handles it automatically
				// depending on the vehicle's velocity

				if (jump_out)
					jump_out_exit();
				else
				{
					vehicle->unk0(true);

					if (vehicle->open_door(VehicleDoor_Left))
						character->get_body_stance()->set_stance(38);

					add_flag2(1 << 6);
					remove_flag2(1 << 4);
					set_timer(1.f);
				}

				break;
			}
			case VehicleSeat_Passenger:
			{
				// exit in the passenger means either normal exit or jump out of the vehicle,
				// it's all about the vehicle velocity so we don't need a different hook
				
				if (vehicle->open_door(VehicleDoor_Right))
					character->get_body_stance()->set_stance(42);

				if (has_flag(1 << 0))
					add_flag(1 << 6);

				add_flag(1 << 5);
				remove_flag(1 << 0);

				jc::write(jc::game::encode_float(1.f), this, jc::vehicle_seat::UNK_FLOAT);

				break;
			}
			}
		}
}

void VehicleSeat::instant_exit()
{
	if (const auto character = get_character())
		if (const auto vehicle = get_vehicle())
			jc::vehicle_seat::hook::instant_exit_hook(vehicle, character, false);
}

void VehicleSeat::jump_out_exit()
{
	if (const auto character = get_character())
		if (const auto vehicle = get_vehicle())
		{
			vehicle->unk1(true);
			vehicle->detach_door(VehicleDoor_Left);

			add_flag2((1 << 6) | (1 << 4));
			set_timer(1.f);
		}
}

void VehicleSeat::add_flag(uint16_t flag)
{
	set_flags(get_flags() | flag);
}

void VehicleSeat::add_flag2(uint32_t flag)
{
	set_flags2(get_flags2() | flag);
}

void VehicleSeat::remove_flag(uint16_t flag)
{
	set_flags(get_flags() & ~flag);
}

void VehicleSeat::remove_flag2(uint32_t flag)
{
	set_flags2(get_flags2() & ~flag);
}

void VehicleSeat::set_flags(uint16_t flags)
{
	jc::write(flags, this, jc::vehicle_seat::FLAGS);
}

void VehicleSeat::set_flags2(uint32_t flags)
{
	jc::write(flags, this, jc::vehicle_seat::FLAGS2);
}

void VehicleSeat::dispatch_entry(Character* character, bool unk)
{
	// we need this because we place hooks on the virtual functions

	switch (get_type())
	{
	case VehicleSeat_Roof:		return jc::vehicle_seat::hook::roof_seat_dispatch_entry_hook(this, character, character->get_controller(), unk);
	case VehicleSeat_Driver:	return jc::vehicle_seat::hook::driver_seat_dispatch_entry_hook(this, character, character->get_controller(), unk);
	case VehicleSeat_Special:	return jc::vehicle_seat::hook::passenger_seat_dispatch_entry_hook(this, character, character->get_controller(), unk);
	case VehicleSeat_Passenger:	return jc::vehicle_seat::hook::special_seat_dispatch_entry_hook(this, character, character->get_controller(), unk);
	}
}

void VehicleSeat::set_timer(float v)
{
	jc::write(v, this, jc::vehicle_seat::TIMER);
}

bool VehicleSeat::is_occupied() const
{
	return jc::v_call<bool>(this, jc::vehicle_seat::vt::IS_OCCUPIED);
}

bool VehicleSeat::is_locked() const
{
	return !jc::read<bool>(this, jc::vehicle_seat::UNLOCKED);
}

bool VehicleSeat::is_instant_exit() const
{
	return jc::read<bool>(this, jc::vehicle_seat::INSTANT_LEAVE);
}

bool VehicleSeat::has_flag(uint16_t flag) const
{
	return get_flags() & flag;
}

bool VehicleSeat::has_flag2(uint32_t flag) const
{
	return get_flags2() & flag;
}

uint8_t VehicleSeat::get_type() const
{
	return static_cast<uint8_t>(jc::v_call<int>(this, jc::vehicle_seat::vt::GET_TYPE));
}

uint16_t VehicleSeat::get_flags() const
{
	return jc::read<uint16_t>(this, jc::vehicle_seat::FLAGS);
}

uint32_t VehicleSeat::get_flags2() const
{
	return jc::read<uint32_t>(this, jc::vehicle_seat::FLAGS2);
}

float VehicleSeat::get_timer() const
{
	return jc::read<float>(this, jc::vehicle_seat::TIMER);
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

ref<VehicleSeat> VehicleSeat::get_ref()
{
	ref<VehicleSeat> r;

	jc::this_call(jc::vehicle_seat::fn::GET_REF, this, &r);

	return r;
}