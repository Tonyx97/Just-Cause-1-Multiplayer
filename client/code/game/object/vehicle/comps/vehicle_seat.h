#pragma once

#include <game/shared/vehicle_seat.h>

namespace jc::vehicle_seat
{
	static constexpr uint32_t CHARACTER			= 0x48;			// Character*
	static constexpr uint32_t ATTACHED_OBJECT	= 0x4C;			// ObjectBase* (either Vehicle or MountedGun I think)
	static constexpr uint32_t UNLOCKED			= 0x150;		// bool
	static constexpr uint32_t INSTANT_LEAVE		= 0x151;		// bool
	static constexpr uint32_t INTERACTABLE		= 0x160;		// Interactable*
	static constexpr uint32_t FLAGS				= 0x194;		// uint16_t
	static constexpr uint32_t UNK_FLOAT			= 0x196;		// uint16_t
	static constexpr uint32_t FLAGS2			= 0x19C;		// uint32_t
	static constexpr uint32_t TIMER				= 0x1A4;		// float
	static constexpr uint32_t WEAPON			= 0x2F8;		// Weapon*

	namespace fn
	{
		static constexpr uint32_t GET_REF			= 0x62F460;
		static constexpr uint32_t RESET_SEAT		= 0x74DE20;
	}

	namespace vt
	{
		static constexpr uint32_t GET_TYPE			= 1;
		static constexpr uint32_t WARP_CHARACTER	= 2;
		static constexpr uint32_t IS_OCCUPIED		= 8;
	}

	namespace hook
	{
		void enable(bool apply);
	}
}

class ObjectBase;
class Interactable;
class Vehicle;
class MountedGun;
class Weapon;

class VehicleSeat
{
private:
public:

	void warp_character(Character* character, bool warp = false);
	void open_door(Character* character);
	void exit(bool jump_out);
	void instant_exit();
	void jump_out_exit();
	void add_flag(uint16_t flag);
	void add_flag2(uint32_t flag);
	void remove_flag(uint16_t flag);
	void remove_flag2(uint32_t flag);
	void set_flags(uint16_t flags);
	void set_flags2(uint32_t flags);
	void dispatch_entry(Character* character, bool unk);
	void set_timer(float v);

	bool is_occupied() const;
	bool is_locked() const;
	bool is_instant_exit() const;
	bool has_flag(uint16_t flag) const;
	bool has_flag2(uint32_t flag) const;

	uint8_t get_type() const;

	uint16_t get_flags() const;
	uint32_t get_flags2() const;

	float get_timer() const;

	ObjectBase* get_attached_object() const;
	Vehicle* get_vehicle() const;
	MountedGun* get_mounted_gun() const;
	Character* get_character() const;
	Weapon* get_weapon() const;

	Interactable* get_interactable() const;

	ref<VehicleSeat> get_ref();
};