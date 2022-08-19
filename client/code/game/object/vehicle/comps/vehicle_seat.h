#pragma once

namespace jc::vehicle_seat
{
	static constexpr uint32_t CHARACTER			= 0x48;			// Character*
	static constexpr uint32_t VEHICLE			= 0x4C;			// Vehicle*
	static constexpr uint32_t INTERACTABLE		= 0x160;		// Interactable*

	namespace vt
	{
		static constexpr uint32_t WARP_CHARACTER = 2;
	}

	namespace hook
	{
		void apply();
		void undo();
	}
}

class Interactable;
class Vehicle;

class VehicleSeat
{
private:
public:

	void warp_character(Character* character);
	void open_door(Character* character);
	void kick_current();

	Vehicle* get_vehicle() const;
	Character* get_character() const;

	Interactable* get_interactable() const;
};