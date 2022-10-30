#include <defs/standard.h>

#include "ammo_manager.h"

#include <game/object/weapon/bullet.h>
#include <game/object/weapon/weapon.h>
#include <game/object/weapon/weapon_belt.h>

#include <game/sys/weapon/weapon_system.h>

namespace jc::ammo_manager
{
	std::unordered_map<Bullet*, Character*> bullet_owners;

	namespace hook
	{
		DEFINE_INLINE_HOOK_IMPL(add_bullet, 0x61F719)
		{
			const auto bullet = ihp->read_ebp<Bullet*>(0x18C);
			const auto weapon = ihp->read_ebp<Weapon*>(0x17C);

			if (bullet && weapon)
				bullet_owners[bullet] = weapon->get_owner();
		}

		DEFINE_INLINE_HOOK_IMPL(remove_bullet, 0x568AD4)
		{
			const auto bullet = ihp->read_ebp<Bullet*>(0x78);

			bullet_owners.erase(bullet);
		}

		void enable(bool apply)
		{
			add_bullet_hook.hook(apply);
			remove_bullet_hook.hook(apply);
		}
	}

	namespace g_fn
	{
		void clear_owner_bullets(Character* character)
		{
			if (character)
				util::container::for_each_safe(bullet_owners, [&](const auto& p)
				{
					return p.second != character;
				});
		}

		void add_bullet_owner(Bullet* bullet, Character* character)
		{
			bullet_owners[bullet] = character;
		}

		Character* get_bullet_owner(Bullet* bullet)
		{
			auto it = bullet_owners.find(bullet);
			return it != bullet_owners.end() ? it->second : nullptr;
		}
	}
}

void AmmoManager::init()
{
}

void AmmoManager::destroy()
{
}

void AmmoManager::set_max_ammo_for_slot(uint8_t slot, int32_t v)
{
	if (slot >= WeaponSlot_A && slot < WeaponSlot_Max)
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

int32_t AmmoManager::get_max_ammo_for_slot(uint8_t slot)
{
	if (slot >= WeaponSlot_A && slot < WeaponSlot_Max)
		return jc::read<int32_t>(this, jc::ammo_manager::MAX_AMMO_SLOTA + (slot * 0x4));

	return -1;
}