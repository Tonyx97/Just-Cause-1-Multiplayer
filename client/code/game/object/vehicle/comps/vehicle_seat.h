#pragma once

namespace jc::vehicle_seat
{
	static constexpr uint32_t CHARACTER = 0x48;		// Character*

	namespace vt
	{
		static constexpr uint32_t WARP_CHARACTER = 2;
	}
}

class VehicleSeat
{
private:
public:

	void warp_character(Character* character);
	void open_door(Character* character);

	Character* get_character() const;
};