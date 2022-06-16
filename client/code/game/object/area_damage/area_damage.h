#pragma once

namespace jc::area_damage
{
	namespace fn
	{
		static constexpr uint32_t CREATE	  = 0x639BE0;
		static constexpr uint32_t ACTIVATE_AT = 0x743640;
	}
}

class AreaDamage
{
private:
	int16_t damage,
		radius;

	int unk;

public:
	AreaDamage(float radius, float damage);

	void activate_at(const vec3& v);
};