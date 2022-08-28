#pragma once

enum VehicleSeatType : uint8_t
{
	VehicleSeat_None,
	VehicleSeat_Roof,
	VehicleSeat_Driver,
	VehicleSeat_Special,
	VehicleSeat_Passenger,
};

enum VehicleSeatFlags : uint16_t
{
	VehicleSeatFlag_MovePassengerToDriver = (1 << 2),
};