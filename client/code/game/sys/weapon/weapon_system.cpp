#include <defs/standard.h>

#include <game/object/base/base.h>

#include "weapon_system.h"

#include <game/sys/resource/model_system.h>

WeaponTemplate* WeaponTemplate::CREATE()
{
	const auto instance = jc::game::malloc<WeaponTemplate>(jc::weapon_system::weapon_template::INSTANCE_SIZE);

	check(instance, "Could not create custom WeaponTemplate");

	jc::this_call(jc::weapon_system::weapon_template::fn::SETUP, instance);

	return instance;
}

void WeaponTemplate::destroy()
{
	// call dtor
	
	jc::this_call(jc::weapon_system::weapon_template::fn::DTOR, this);

	// free memory

	jc::game::free(this);
}

void WeaponTemplate::init_from_map(object_base_map* map)
{
	jc::this_call(jc::weapon_system::weapon_template::fn::INIT_FROM_MAP, this, map);
}

void WeaponTemplate::set_ammo_max_range(float v)
{
	jc::write(v, this, jc::weapon_system::weapon_template::AMMO_MAX_RANGE);
}

void WeaponTemplate::set_ammo_velocity(float v)
{
	jc::write(v, this, jc::weapon_system::weapon_template::AMMO_VELOCITY);
}

uint32_t WeaponTemplate::get_id() const
{
	return jc::read<uint32_t>(this, jc::weapon_system::weapon_template::ID);
}

uint32_t WeaponTemplate::get_slot() const
{
	return jc::read<uint32_t>(this, jc::weapon_system::weapon_template::SLOT);
}

float WeaponTemplate::get_ammo_max_range() const
{
	return jc::read<float>(this, jc::weapon_system::weapon_template::AMMO_MAX_RANGE);
}

float WeaponTemplate::get_ammo_velocity() const
{
	return jc::read<float>(this, jc::weapon_system::weapon_template::AMMO_VELOCITY);
}

std::string WeaponTemplate::get_model() const
{
	return jc::read<std::string>(this, jc::weapon_system::weapon_template::MODEL_NAME);
}

std::string WeaponTemplate::get_type_name() const
{
	return jc::read<std::string>(this, jc::weapon_system::weapon_template::TYPE_NAME);
}

std::string WeaponTemplate::get_name() const
{
	return jc::read<std::string>(this, jc::weapon_system::weapon_template::NAME);
}

void WeaponSystem::init()
{
	// increase all weapon's max range by 100 times
	
	for_each_weapon_template([](int, WeaponTemplate* weapon_template)
	{
		weapon_template->set_ammo_max_range(weapon_template->get_ammo_max_range() * 100.f);

		switch (weapon_template->get_id())
		{
		case Weapon_M60:
		case Weapon_AIRPLANE_GUNS:
		case Weapon_Car_guns:
		case Weapon_SIDEWINDER:
		case Weapon_Helicopter_gun:
		{
			weapon_template->set_ammo_velocity(weapon_template->get_ammo_velocity() * 5.f);
			break;
		}
		}
	});
}

void WeaponSystem::destroy()
{
}

void WeaponSystem::dump()
{
	// enum

	log(RED, "enum WeaponID");
	log(RED, "{{");
	log(GREEN, "Weapon_None,");

	std::unordered_map<uint32_t, std::string> fixed_names;

	for_each_weapon_template([&](int, WeaponTemplate* t)
	{
		auto name = t->get_type_name();
		auto id = t->get_id();

		std::replace(name.begin(), name.end(), ' ', '_');
		
		std::erase(name, '(');
		std::erase(name, ')');

		fixed_names.insert({ id, "Weapon_" + name });

		log(GREEN, "Weapon_{} = {}, // {} | {}", name, id, t->get_name(), t->get_type_name());
	});

	log(RED, "}};");

	// weapons id to type name

	log(RED, "inline const std::unordered_map<uint32_t, std::string> weapons_id_to_type_name =");
	log(RED, "{{");

	for_each_weapon_template([&](int, WeaponTemplate* t)
	{
		auto name = t->get_type_name();
		auto id = t->get_id();

		log(GREEN, "{{ {}, R\"{}\" }},", fixed_names[id], name);
	});

	log(RED, "}};");

	// weapons id to model name

	log(RED, "inline const std::unordered_map<uint32_t, std::string> weapons_id_to_model_name =");
	log(RED, "{{");

	for_each_weapon_template([&](int, WeaponTemplate* t)
	{
		auto name = t->get_model();
		auto id = t->get_id();

		if (!name.empty())
			log(GREEN, "{{ {}, R\"({})\" }},", fixed_names[id], name);
	});

	log(RED, "}};");

	// weapons id to slot

	log(RED, "inline const std::unordered_map<uint32_t, int32_t> weapons_id_to_type =");
	log(RED, "{{");

	for_each_weapon_template([&](int, WeaponTemplate* t)
	{
		auto name = t->get_type_name();
		auto id = t->get_id();

		log(GREEN, "{{ {}, {} }},", fixed_names[id], t->get_slot());
	});

	log(RED, "}};");
}

void WeaponSystem::insert_weapon_template(WeaponTemplate* weapon_template)
{
	jc::this_call(jc::weapon_system::fn::INSERT_WEAPON_TEMPLATE, get_template_list(), &weapon_template);
}

void WeaponSystem::remove_weapon_template(WeaponTemplate* weapon_template)
{
	const auto template_list = get_template_list();

	if (const auto it = template_list->find(weapon_template); it != template_list->end())
	{
		template_list->erase(it);

		weapon_template->destroy();
	}
}

void WeaponSystem::remove_weapon_template(uint32_t id)
{
	if (const auto weapon_template = get_weapon_template_from_id(id))
		remove_weapon_template(weapon_template);
}

WeaponTemplate* WeaponSystem::create_weapon_template(object_base_map* map)
{
	if (const auto weapon_template = WeaponTemplate::CREATE())
	{
		weapon_template->init_from_map(map);

		insert_weapon_template(weapon_template);

		return weapon_template;
	}

	return nullptr;
}

int WeaponSystem::for_each_weapon_template(const weapon_template_iteration_t& fn)
{
	const auto template_list = get_template_list();

	int32_t count = 0;

	for (auto weapon_template : *template_list)
		fn(count++, weapon_template);

	return count;
}

int32_t WeaponSystem::get_weapon_type(uint8_t id) const
{
	auto it = jc::vars::weapons_id_to_type.find(id);
	return it != jc::vars::weapons_id_to_type.end() ? it->second : -1;
}

jc::stl::vector_ptr<WeaponTemplate*>* WeaponSystem::get_template_list() const
{
	return REF(jc::stl::vector_ptr<WeaponTemplate*>*, this, jc::weapon_system::TEMPLATE_LIST);
}

WeaponTemplate* WeaponSystem::get_weapon_template_from_id(uint32_t id) const
{
	const auto template_list = get_template_list();

	for (auto weapon_template : *template_list)
		if (weapon_template->get_id() == id)
			return weapon_template;

	return nullptr;
}

std::string WeaponSystem::get_weapon_typename(uint8_t id)
{
	auto it = jc::vars::weapons_id_to_type_name.find(id);
	return it != jc::vars::weapons_id_to_type_name.end() ? it->second : "";
}

std::string WeaponSystem::get_weapon_model(uint8_t id)
{
	auto it = jc::vars::weapons_id_to_model_name.find(id);
	return it != jc::vars::weapons_id_to_model_name.end() ? it->second : "";
}

shared_ptr<Weapon> WeaponSystem::create_weapon_instance(const std::string& name)
{
	shared_ptr<Weapon> p;

	const jc::stl::string _name = name;

	ModelRestorationPatch patch {};

	jc::this_call(jc::weapon_system::fn::ALLOC_WEAPON_BY_NAME, this, &p, &_name);

	return p;
}

shared_ptr<Weapon> WeaponSystem::create_weapon_instance(uint8_t id)
{
	shared_ptr<Weapon> p;

	ModelRestorationPatch patch {};

	jc::this_call(jc::weapon_system::fn::ALLOC_WEAPON_BY_ID, this, &p, static_cast<int32_t>(id));

	return p;
}