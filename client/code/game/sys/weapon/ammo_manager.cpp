#include <defs/standard.h>

#include "ammo_manager.h"

#include <game/object/weapon/weapon.h>
#include <game/object/weapon/weapon_belt.h>

#include <game/sys/weapon/weapon_system.h>

void AmmoManager::init()
{
}

void AmmoManager::destroy()
{
}

void AmmoManager::set_max_ammo_for_slot(WeaponSlot slot, int32_t v)
{
	if (slot >= WeaponSlot_A && slot <= WeaponSlot_H)
		return jc::write<int32_t>(v, this, jc::ammo_manager::MAX_AMMO_SLOTA + (slot * 0x4));
}

int AmmoManager::for_each_bullet(const bullet_iteration_t& fn)
{
	auto bullet_list = jc::read<jc::stl::vector<Bullet*>>(this, jc::ammo_manager::BULLET_LIST);

	int i = 0;

	for (auto bullet : bullet_list)
		fn(i++, bullet);

	return i;
}

int32_t AmmoManager::get_max_ammo_for_slot(WeaponSlot slot)
{
	if (slot >= WeaponSlot_A && slot <= WeaponSlot_H)
		return jc::read<int32_t>(this, jc::ammo_manager::MAX_AMMO_SLOTA + (slot * 0x4));

	return -1;
}