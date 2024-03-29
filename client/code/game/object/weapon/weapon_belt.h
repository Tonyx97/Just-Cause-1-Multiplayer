#pragma once

namespace jc::weapon_belt
{
	static constexpr uint32_t AMMO_BEGIN				= 0x4; // int32_t
	static constexpr uint32_t WEAPON_LIST				= 0x50; // list of weapons, each entry is 0x10
	static constexpr uint32_t CURRENT_WEAPON_SLOT_ID	= 0xF0; // int16_t
	static constexpr uint32_t DRAW_WEAPON_ID			= 0xF4; // int16_t

	namespace fn
	{
		static constexpr uint32_t ADD_WEAPON_AMMO		= 0x60C300;
		static constexpr uint32_t ADD_WEAPON			= 0x60CC00;
		static constexpr uint32_t GET_CURRENT_WEAPON	= 0x60D7D0;
		static constexpr uint32_t GET_SLOT_BY_TYPE_ID	= 0x60CA60;
		static constexpr uint32_t CLEAR					= 0x60BAA0;
		static constexpr uint32_t REMOVE_WEAPON			= 0x8B8D90;
		
	}
}

class Weapon;

class WeaponBelt
{
public:

	static constexpr auto MAX_SLOTS() { return 10; }

	void set_weapon_ammo(int32_t bullet_type, int32_t v);
	void add_weapon_ammo(int32_t bullet_type, int32_t v);
	void remove_weapon(int32_t slot);
	void clear();

	bool has_weapon(int32_t id);

	shared_ptr<Weapon> get_weapon_from_slot(int32_t slot);
	shared_ptr<Weapon> get_weapon_from_type(uint8_t type);

	int16_t get_current_weapon_slot_id() const;
	int16_t get_draw_weapon_slot_id() const;

	int32_t get_weapon_slot(const shared_ptr<Weapon>& weapon) const;
	int32_t get_weapon_slot(int32_t type) const;
	int32_t get_weapon_ammo(int32_t bullet_type) const;

	shared_ptr<Weapon> get_current_weapon();
	shared_ptr<Weapon> add_weapon(shared_ptr<Weapon>& weapon);
	shared_ptr<Weapon> add_weapon(uint8_t weapon_id);
};