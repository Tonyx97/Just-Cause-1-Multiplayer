#include <defs/standard.h>

#include "weapon_belt.h"
#include "weapon.h"

#include <game/sys/weapon_system.h>

void WeaponBelt::set_slot_ammo(WeaponSlot slot, int32_t v)
{
	if (slot >= WeaponSlot_A && slot <= WeaponSlot_H)
		jc::write(v, this, jc::weapon_belt::AMMO_SLOTA + (slot * 0x4));
}

int32_t WeaponBelt::get_slot_ammo(WeaponSlot slot)
{
	if (slot >= WeaponSlot_A && slot <= WeaponSlot_H)
		return jc::read<int32_t>(this, jc::weapon_belt::AMMO_SLOTA + (slot * 0x4));

	return -1;
}

int32_t WeaponBelt::get_current_weapon_id()
{
	return jc::read<int32_t>(this, jc::weapon_belt::CURRENT_WEAPON_ID);
}

ref<Weapon> WeaponBelt::get_current_weapon()
{
	ref<Weapon> r;

	jc::this_call(jc::weapon_belt::fn::GET_CURRENT_WEAPON, this, &r);

	return r;
}