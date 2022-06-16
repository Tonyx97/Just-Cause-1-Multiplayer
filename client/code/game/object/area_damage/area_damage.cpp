#include <defs/standard.h>

#include "area_damage.h"

AreaDamage::AreaDamage(float radius, float damage)
{
	jc::this_call<AreaDamage*>(jc::area_damage::fn::CREATE, this, radius, damage);
}

void AreaDamage::activate_at(const vec3& v)
{
	jc::c_call<AreaDamage*>(jc::area_damage::fn::ACTIVATE_AT, this, v.x, v.y, v.z);
}