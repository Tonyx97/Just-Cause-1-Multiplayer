#pragma once

#include <game/shared/vehicle_seat.h>

namespace jc::vehicle_seat
{
	static constexpr uint32_t CHARACTER			= 0x48;			// Character*
	static constexpr uint32_t VEHICLE			= 0x4C;			// Vehicle*
	static constexpr uint32_t INTERACTABLE		= 0x160;		// Interactable*
	static constexpr uint32_t FLAGS				= 0x194;		// uint16_t
	static constexpr uint32_t FLAGS2			= 0x19C;		// uint32_t
	static constexpr uint32_t TIMER				= 0x1A4;		// float

	namespace fn
	{
		static constexpr uint32_t GET_REF		= 0x62F460;
	}

	namespace vt
	{
		static constexpr uint32_t GET_TYPE			= 1;
		static constexpr uint32_t WARP_CHARACTER	= 2;
		static constexpr uint32_t IS_OCCUPIED		= 8;
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

	void warp_character(Character* character, bool warp = false);
	void open_door(Character* character);
	void kick_current(bool instant);
	void add_flag(uint16_t flag);
	void add_flag2(uint32_t flag);
	void remove_flag(uint16_t flag);
	void remove_flag2(uint32_t flag);
	void set_flags(uint16_t flags);
	void set_flags2(uint32_t flags);
	void dispatch_entry(Character* character, bool unk);
	void set_timer(float v);

	bool is_occupied() const;

	uint8_t get_type() const;

	uint16_t get_flags() const;
	uint32_t get_flags2() const;

	float get_timer() const;

	Vehicle* get_vehicle() const;
	Character* get_character() const;

	Interactable* get_interactable() const;

	ref<VehicleSeat> get_ref();
};