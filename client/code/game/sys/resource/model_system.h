#pragma once

namespace jc::model_system
{
	static constexpr uint32_t SINGLETON = 0xD84F50; // ModelSystem*

	namespace fn
	{
		static constexpr uint32_t LOAD_RBM_FROM_MEM			= 0x57A070;
	}
}

class ModelSystem
{
public:

	void init();
	void destroy();

	bool load_rbm(const std::string& filename);
	bool unload_rbm(const std::string& filename);
};

inline Singleton<ModelSystem, jc::model_system::SINGLETON> g_model_system;