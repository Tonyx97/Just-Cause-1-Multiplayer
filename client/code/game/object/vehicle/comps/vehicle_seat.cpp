#include <defs/standard.h>

#include "vehicle_seat.h"

#include <game/object/character/character.h>

void VehicleSeat::warp_character(Character* character)
{
	jc::v_call(this, jc::vehicle_seat::vt::WARP_CHARACTER, character, character->get_handle_base(), false);

	//jc::this_call(0x5A1D40, character, true);
}

void VehicleSeat::open_door(Character* character)
{
	jc::v_call(this, jc::vehicle_seat::vt::WARP_CHARACTER, character, character->get_handle_base(), true);
}

Character* VehicleSeat::get_character() const
{
	return jc::read<Character*>(this, jc::vehicle_seat::CHARACTER);
}