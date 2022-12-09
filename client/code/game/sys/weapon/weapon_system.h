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