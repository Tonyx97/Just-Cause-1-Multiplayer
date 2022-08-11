#pragma once

namespace jc::vehicle_manager
{
	static constexpr uint32_t SINGLETON = 0xD84F88; // VehicleManager*
}

using vehicle_iteration_t = std::function<void(int, void*)>;

class VehicleManager
{
public:
	void init();
	void destroy();

	int for_each_vehicle(const vehicle_iteration_t& fn);
};

inline Singleton<VehicleManager, jc::vehicle_manager::SINGLETON> g_vehicle;