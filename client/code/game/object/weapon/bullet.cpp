#include <defs/standard.h>

#include "bullet.h"

#include <game/sys/resource/physics.h>

namespace jc::bullet::hook
{
	DEFINE_HOOK_THISCALL(step, 0x6466F0, void, Bullet* bullet, float delta)
	{
		step_hook(bullet, delta);

		// custom bullet drop calculation
		
		const auto gravity = g_physics->get_gravity();
		const auto direction = bullet->get_direction();
		const auto velocity = bullet->get_velocity();

		vec3 new_direction =
		{
			(direction.x * velocity) + gravity.x * 2.f * delta,
			(direction.y * velocity) + gravity.y * 2.f * delta,
			(direction.z * velocity) + gravity.z * 2.f * delta
		};

		bullet->set_velocity(glm::length(new_direction));
		bullet->set_direction(new_direction);
	}
}

void jc::bullet::hook::enable(bool apply)
{
	step_hook.hook(apply);
}

void Bullet::calculate_drop(float delta)
{
	jc::v_call(this, jc::bullet::vt::CALCULATE_DROP, delta);
}

void Bullet::set_position(const vec3& v)
{
	jc::write(v, this, jc::bullet::POSITION);
}

void Bullet::set_velocity(float v)
{
	jc::write(jc::game::encode_float(v), this, jc::bullet::VELOCITY);
}

void Bullet::set_direction(const vec3& v)
{
	jc::write(glm::normalize(v), this, jc::bullet::DIRECTION);
}

bool Bullet::is_alive()
{
	return jc::read<bool>(this, jc::bullet::IS_ALIVE);
}

float Bullet::get_damage()
{
	return jc::read<float>(this, jc::bullet::DAMAGE);
}

float Bullet::get_max_range()
{
	return jc::read<float>(this, jc::bullet::MAX_RANGE);
}

float Bullet::get_velocity()
{
	return jc::game::decode_float(jc::read<uint16_t>(this, jc::bullet::VELOCITY));
}

vec3 Bullet::get_position()
{
	return jc::read<vec3>(this, jc::bullet::POSITION);
}

vec3 Bullet::get_direction()
{
	return jc::read<vec3>(this, jc::bullet::DIRECTION);
}

vec3 Bullet::get_last_position()
{
	return jc::read<vec3>(this, jc::bullet::LAST_POSITION);
}