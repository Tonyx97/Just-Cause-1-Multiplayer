#pragma once

DEFINE_ENUM(VehicleSeatType, uint8_t)
{
	VehicleSeat_None,
	VehicleSeat_Roof,
	VehicleSeat_Driver,
	VehicleSeat_Special,
	VehicleSeat_Passenger,
};

DEFINE_ENUM(VehicleSeatFlags, uint16_t)
{
	VehicleSeatFlag_PassengerToDriverSeat		= (1 << 2),
	VehicleSeatFlag_DriverToRoofSeat			= (1 << 5),
};