#pragma once

#include "resource.h"

#include "../vars/exported_entities.h"

namespace jc::ee_resource
{
	static constexpr uint32_t EXPORTED_ENTITY_RESOURCE = 0x118;

	static constexpr uint32_t INSTANCE_SIZE = 0x120;

	namespace fn
	{
		static constexpr uint32_t SETUP					= 0x761700;
		static constexpr uint32_t SET_FLAG				= 0x5C23C0;
		static constexpr uint32_t CHECK					= 0x5C17B0;
		static constexpr uint32_t FINISH_LOAD			= 0x7618A0;
		static constexpr uint32_t MARK_AS_LOADED		= 0x761840;
		static constexpr uint32_t LOADING_ERROR			= 0x761880;
	}
}

class ExportedEntity;

class ExportedEntityResource : public Resource
{
public:

	static ExportedEntityResource* CREATE();

	static constexpr auto SIZE() { return jc::ee_resource::INSTANCE_SIZE; }

	void set_flag(uint32_t flag);

	bool check() const;
	bool is_loaded() const;

	ExportedEntity* get_exported_entity() const;
};