#pragma once

namespace jc::particle_system
{
	static constexpr uint32_t SINGLETON = 0xD84F38; // ParticleSystem*

	namespace fn
	{
		static constexpr uint32_t SPAWN_FX = 0x574F70;
	}
}

class ParticleSystem
{
public:
	void init();
	void destroy();

	void* spawn_fx(const std::string& name, const vec3& position, const vec3& rotation = {}, const vec3& velocity = {}, bool unk1 = false, bool unk2 = false);
};

inline ParticleSystem* g_particle = nullptr;