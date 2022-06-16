#include <defs/standard.h>

#include "bullet.h"

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