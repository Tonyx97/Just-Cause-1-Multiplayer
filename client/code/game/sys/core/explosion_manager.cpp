#include <defs/standard.h>

#include "explosion_manager.h"

#include "../particle/particle_system.h"
#include "../resource/physics.h"

#include <game/object/area_damage/area_damage.h>

void ExplosionManager::init()
{
}

void ExplosionManager::destroy()
{
}

void ExplosionManager::set_grenade_timer(float v)
{
	jc::write(v, jc::explosion_manager::GRENADE_TIMER);
}

bool ExplosionManager::create_explosion(const vec3& position, float radius, float force, float damage)
{
	const std::string default_name = "360_exp_4_1";

	AreaDamage area_dmg(radius, damage);

	area_dmg.activate_at(position);

	g_physics->create_force_pulse(position, radius, force);
	g_particle->spawn_fx(default_name, position, {}, {}, true);

	return false;
}

float ExplosionManager::get_grenade_timer() const
{
	return jc::read<float>(jc::explosion_manager::GRENADE_TIMER);
}