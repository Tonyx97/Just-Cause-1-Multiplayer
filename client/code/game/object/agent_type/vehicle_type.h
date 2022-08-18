#pragma once

#include "../base/base.h"

namespace jc::vehicle_type
{
	static constexpr uint32_t EE_RESOURCE = 0x10C;
	static constexpr uint32_t IS_LOADED = 0x1D5;

	static constexpr uint32_t INSTANCE_SIZE = 0x1F8;

	namespace fn
	{
		static constexpr uint32_t CREATE			= 0x671B40;
		static constexpr uint32_t SETUP				= 0x817370;
		static constexpr uint32_t LOAD				= 0x818350;
		static constexpr uint32_t CREATE_VEHICLE	= 0x8184E0;
	}
}

class ExportedEntityResource;
class Vehicle;

class VehicleType : public ObjectBase
{
public:

	static VehicleType* CREATE();

	void load(const std::string& class_name, const std::string& ee_name, object_base_map* map);

	bool is_loaded() const;

	ExportedEntityResource* get_ee_resource() const;

	ref<Vehicle> create_vehicle(const Transform& transform);
};