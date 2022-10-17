#pragma once

#include <game/object/asset/asset_texture.h>

#include "resource_cache.h"

namespace jc::texture_system
{
	static constexpr uint32_t SINGLETON = 0xAF2410; // TextureSystem*

	static constexpr uint32_t TEXTURE_CACHE = 0x10;

	namespace vt
	{
		static constexpr uint32_t LOAD_TEXTURE_FROM_MEM = 16;
	}
}

class TextureSystem : public ResourceCache<AssetTexture, jc::texture_system::TEXTURE_CACHE>
{
public:

	void init();
	void destroy();

	bool load_texture(const std::string& filename);
	bool unload_texture(const std::string& filename);
};

inline Singleton<TextureSystem, jc::texture_system::SINGLETON> g_texture_system;