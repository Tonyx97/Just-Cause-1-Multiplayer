#include <defs/standard.h>

#include "weapon_system.h"

int WeaponTemplate::get_id() const
{
	return jc::read<int>(this, jc::weapon_system::weapon_template::ID);
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
}

void WeaponSystem::destroy()
{
}

void WeaponSystem::dump()
{
	log(RED, "enum WeaponID");
	log(RED, "{{");
	log(GREEN, "Weapon_None,");

	for_each_weapon_template([](int, WeaponTemplate* t)
		{
		auto name = t->get_type_name();
		auto id = t->get_id();

		std::replace(name.begin(), name.end(), ' ', '_');
		
		std::erase(name, '(');
		std::erase(name, ')');

		log(GREEN, "Weapon_{} = {}, // {}", name, id, t->get_name()); });

	log(RED, "}};");
}

int WeaponSystem::for_each_weapon_template(const weapon_template_iteration_t& fn)
{
	const auto template_list = REF(std::vector<WeaponTemplate*>*, this, jc::weapon_system::TEMPLATE_LIST);

	int count = 0;

	for (auto weapon_template : *template_list)
		fn(count++, weapon_template);

	return count;
}
/*
sptr<Weapon> WeaponSystem::create_weapon_instance(const std::string& name)
{
	sptr<Weapon> p;

	jc::this_call<sptr<Weapon>*>(jc::weapon_system::fn::ALLOC_WEAPON_BY_NAME, this, &p, &name);

	return std::move(p);
}

sptr<Weapon> WeaponSystem::create_weapon_instance(WeaponID id)
{
	sptr<Weapon> p;

	jc::this_call<sptr<Weapon>*>(jc::weapon_system::fn::ALLOC_WEAPON_BY_ID, this, &p, id);

	return std::move(p);
}*/