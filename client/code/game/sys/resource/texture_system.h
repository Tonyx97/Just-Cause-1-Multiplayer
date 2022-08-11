#pragma once

namespace jc::texture_system
{
	static constexpr uint32_t SINGLETON = 0xAF2410; // TextureSystem*

	namespace vt
	{
		static constexpr uint32_t LOAD_TEXTURE_FROM_MEM = 16;
	}
}

class TextureSystem
{
public:

	void init();
	void destroy();

	bool load_texture(const std::string& filename);
	bool unload_texture(const std::string& filename);
};

inline Singleton<TextureSystem, jc::texture_system::SINGLETON> g_texture_system;