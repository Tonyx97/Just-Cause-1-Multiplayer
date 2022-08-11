#pragma once

namespace jc::anim_system
{
	static constexpr uint32_t SINGLETON = 0xD84D14; // ModelSystem*

	namespace fn
	{
		static constexpr uint32_t LOAD_ANIM_FROM_MEM = 0x55F170;
	}
}

class AnimSystem
{
public:

	void init();
	void destroy();

	bool load_anim(const std::string& filename);
	bool unload_anim(const std::string& filename);
};

inline Singleton<AnimSystem, jc::anim_system::SINGLETON> g_anim_system;