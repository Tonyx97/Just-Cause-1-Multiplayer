#pragma once

#include <game/object/vars/weapons.h>

namespace jc::weapon_system
{
	static constexpr uint32_t SINGLETON = 0xD84F98; // WeaponSystem*

	static constexpr uint32_t TEMPLATE_LIST = 0x4; // std::vector<WeaponTemplate*>

	namespace fn
	{
		static constexpr uint32_t ALLOC_WEAPON_BY_NAME = 0x57E850;
		static constexpr uint32_t ALLOC_WEAPON_BY_ID   = 0x57EA00;
	}

	namespace weapon_template
	{
		static constexpr uint32_t ID		 = 0x0; // int
		static constexpr uint32_t MODEL_NAME = 0x14; // std::string
		static constexpr uint32_t TYPE_NAME	 = 0x50; // std::string
		static constexpr uint32_t NAME		 = 0x6C; // std::string
	}
}

class WeaponTemplate
{
public:
	uint32_t get_id() const;

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

	int for_each_weapon_template(const weapon_template_iteration_t& fn);

	std::string get_weapon_typename(uint32_t id);
	std::string get_weapon_model(uint32_t id);

	//sptr<Weapon> create_weapon_instance(const std::string& name);
	//sptr<Weapon> create_weapon_instance(WeaponID id);
};

inline WeaponSystem* g_weapon = nullptr;