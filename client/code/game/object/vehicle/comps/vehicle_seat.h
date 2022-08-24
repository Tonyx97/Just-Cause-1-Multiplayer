#pragma once

namespace jc::vehicle_seat
{
	static constexpr uint32_t CHARACTER			= 0x48;			// Character*
	static constexpr uint32_t VEHICLE			= 0x4C;			// Vehicle*
	static constexpr uint32_t INTERACTABLE		= 0x160;		// Interactable*

	namespace vt
	{
		static constexpr uint32_t GET_TYPE			= 1;
		static constexpr uint32_t WARP_CHARACTER	= 2;
	}

	namespace hook
	{
		void apply();
		void undo();
	}
}

class Interactable;
class Vehicle;

enum VehicleSeatType : uint8_t
{
	VehicleSeat_None,
	VehicleSeat_Roof,
	VehicleSeat_Driver,
	VehicleSeat_Special,
	VehicleSeat_Passenger,
};

class VehicleSeat
{
private:
public:

	void warp_character(Character* character, bool warp = false);
	void open_door(Character* character);
	void kick_current(bool instant);

	uint8_t get_type() const;

	Vehicle* get_vehicle() const;
	Character* get_character() const;

	Interactable* get_interactable() const;
};