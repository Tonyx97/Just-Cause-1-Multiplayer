#pragma once

#include <game/object/asset/asset_rbm.h>

#include "resource_cache.h"

namespace jc::model_system
{
	static constexpr uint32_t SINGLETON = 0xD84F50; // ModelSystem*

	static constexpr uint32_t MODEL_CACHE = 0x4;

	namespace fn
	{
		static constexpr uint32_t LOAD_RBM_FROM_MEM			= 0x57A070;
	}
}

class ModelSystem : public ResourceCache<AssetRBM, jc::model_system::MODEL_CACHE>
{
public:

	void init();
	void destroy();

	bool load_rbm(const std::string& filename);
	bool load_rbm(const std::string& filename, const std::vector<uint8_t>& data);
	bool unload_rbm(const std::string& filename);
};

inline Singleton<ModelSystem, jc::model_system::SINGLETON> g_model_system;