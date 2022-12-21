#pragma once

#include <game/object/vars/weapons.h>

namespace jc::weapon_system
{
	static constexpr uint32_t SINGLETON = 0xD84F98; // WeaponSystem*

	static constexpr uint32_t TEMPLATE_LIST = 0x4; // std::vector<WeaponTemplate*>

	namespace fn
	{
		static constexpr uint32_t ALLOC_WEAPON_BY_NAME		= 0x57E850;
		static constexpr uint32_t ALLOC_WEAPON_BY_ID		= 0x57EA00;
		static constexpr uint32_t WEAPON_TEMPLATE_SETUP		= 0x713D00;
		static constexpr uint32_t INSERT_WEAPON_TEMPLATE	= 0x57ED80;
	}

	namespace weapon_template
	{
		static constexpr uint32_t ID				= 0x0;		// int
		static constexpr uint32_t SLOT				= 0xC;		// int
		static constexpr uint32_t MODEL_NAME		= 0x14;		// std::string
		static constexpr uint32_t TYPE_NAME			= 0x50;		// std::string
		static constexpr uint32_t NAME				= 0x6C;		// std::string
		static constexpr uint32_t AMMO_VELOCITY		= 0x128;	// float
		static constexpr uint32_t AMMO_MAX_RANGE	= 0x12C;	// float

		static constexpr uint32_t INSTANCE_SIZE = 0x17C;

		namespace fn
		{
			static constexpr uint32_t DTOR				= 0x7141C0;
			static constexpr uint32_t FREE				= 0x713D00;
			static constexpr uint32_t SETUP				= 0x713D00;
			static constexpr uint32_t INIT_FROM_MAP		= 0x7142B0;
		}
	}
}

struct object_base_map;

class WeaponTemplate
{
public:

	enum Hash : uint32_t
	{
		accuracy_ai = 0x43d3fa78,
		accuracy_player = 0xc9dae566,
		animtype = 0x2caec4c6,
		bulletspershot = 0x67f4b92,
		bullettype = 0x73c279d9,
		fireeffectid1 = 0xa9818615,
		fireeffectpos_x = 0xfde6d38c,
		fireeffectpos_y = 0xe807fbbe,
		fireeffectpos_z = 0x83fcbe6d,
		firerate = 0x3c973815,
		id = 0xc17ff89f,
		icon_index = 0x983bdc19,
		magazinesize = 0x7932fd00,
		maxaimangledeg = 0x9db0dcdf,
		minaimangledeg = 0xfdc2fc1f,
		model = 0xdfe26313,
		name = 0xb8fbd88e,
		display_name = 0xb231ec06,
		reloadtime = 0x737cf1df,
		shelleffectid1 = 0x520ae427,
		shelleffectpos_x = 0xdf4dbe38,
		shelleffectpos_y = 0x40c4076a,
		shelleffectpos_z = 0xa286f163,
		soundweaponid = 0xb48c8992,
		soundclickid = 0x2ecc28f9,
		soundfireid = 0xdc4e8e89,
		soundreloadid = 0x3c6afa74,
		weapontype = 0xea9b2d84,
		silencer = 0xe43f6ff8,
		ammo_velocity = 0x3ae7b611,
		ammo_max_range = 0xb27fbdf,
		aim_max_range_ai = 0x51905fda,
		aim_max_range_player = 0x8dccf8a,
		ammo_max_damage_ai = 0x72fdb359,
		ammo_max_damage_player = 0x61d710ac,
		ammo_area_damage_radius = 0x58dd9170,
		play_recoil_on_release = 0xbebf734b,
		miss_min_distance = 0x6e24e123,
		miss_max_distance = 0xe775a5da,
		miss_shrink_distance = 0x2524eee2,
		min_burst_bullets = 0xebb4f93f,
		max_burst_bullets = 0xf776041b,
		min_burst_delay = 0xc5a583e4,
		max_burst_delay = 0x2419daa1,
		use_against_vehicles = 0x2e3e2094,
		click_fire = 0x92d1b4b0,
		min_click_delay = 0xf400ec2a,
		max_click_delay = 0x88bbee10,
		ammo_explosion_effect = 0xb3776904
	};

	static WeaponTemplate* CREATE();

	void destroy();
	void init_from_map(object_base_map* map);
	void set_ammo_max_range(float v);
	void set_ammo_velocity(float v);

	uint32_t get_id() const;
	uint32_t get_slot() const;

	float get_ammo_max_range() const;
	float get_ammo_velocity() const;

	std::string get_model() const;
	std::string get_type_name() const;
	std::string get_name() const;
};

using weapon_template_iteration_t = std::function<void(int, WeaponTemplate*)>;

class Weapon;

class WeaponSystem
{
public:

	void init();
	void destroy();
	void dump();
	void insert_weapon_template(WeaponTemplate* weapon_template);
	void remove_weapon_template(WeaponTemplate* weapon_template);
	void remove_weapon_template(uint32_t id);

	WeaponTemplate* create_weapon_template(object_base_map* map);

	int32_t for_each_weapon_template(const weapon_template_iteration_t& fn);
	int32_t get_weapon_type(uint8_t id) const;

	jc::stl::vector_ptr<WeaponTemplate*>* get_template_list() const;

	WeaponTemplate* get_weapon_template_from_id(uint32_t id) const;

	std::string get_weapon_typename(uint8_t id);
	std::string get_weapon_model(uint8_t id);

	shared_ptr<Weapon> create_weapon_instance(const std::string& name);
	shared_ptr<Weapon> create_weapon_instance(uint8_t id);

	template <typename Fn>
	void for_each_weapon_type(const Fn& fn)
	{
		for (auto type : jc::vars::weapon_types)
			fn(type);
	}
};

inline Singleton<WeaponSystem, jc::weapon_system::SINGLETON> g_weapon;