#pragma once

#include "../base/base.h"

namespace jc::spawn_point
{
	static constexpr uint32_t TRANSFORM	  = 0x28; // transform
	static constexpr uint32_t POSITION	  = 0x58; // vec3
	static constexpr uint32_t SPAWN_NAME  = 0x6C; // std::string
	static constexpr uint32_t PRESET_NAME = 0x88; // std::string

	namespace vt
	{
		static constexpr uint32_t SPAWN			 = 33;
		static constexpr uint32_t SPAWN_INTERNAL = 40;
	}
}

class SpawnPoint : public ObjectBase
{
public:
	void spawn();
	void spawn_internal();
};