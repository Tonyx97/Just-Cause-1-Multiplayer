#pragma once

namespace jc::explosion_manager
{
	static constexpr uint32_t GRENADE_TIMER = 0xAEDFD0; // float
}

class ExplosionManager
{
public:
	void init();
	void destroy();
	void set_grenade_timer(float v);

	bool create_explosion(const vec3& position, float radius, float force, float damage = 0.f);

	float get_grenade_timer() const;
};

inline ExplosionManager* g_explosion = nullptr;