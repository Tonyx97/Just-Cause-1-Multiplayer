#pragma once

namespace jc::weapon_belt
{
	static constexpr uint32_t AMMO_SLOTA		= 0x8; // int32_t
	static constexpr uint32_t AMMO_SLOTB		= 0xC; // int32_t
	static constexpr uint32_t AMMO_SLOTC		= 0x10; // int32_t
	static constexpr uint32_t AMMO_SLOTD		= 0x14; // int32_t
	static constexpr uint32_t AMMO_SLOTE		= 0x18; // int32_t
	static constexpr uint32_t AMMO_SLOTF		= 0x1C; // int32_t
	static constexpr uint32_t AMMO_SLOTG		= 0x20; // int32_t
	static constexpr uint32_t AMMO_SLOTH		= 0x24; // int32_t
	static constexpr uint32_t CURRENT_WEAPON_ID = 0xF0; // int32_t

	namespace fn
	{
		static constexpr uint32_t GET_CURRENT_WEAPON = 0x60D7D0;
	}
}

class Weapon;

enum WeaponSlot;

class WeaponBelt
{
public:
	void set_slot_ammo(WeaponSlot slot, int32_t v);

	int32_t get_slot_ammo(WeaponSlot slot);
	int32_t get_current_weapon_id();

	ref<Weapon> get_current_weapon();
};