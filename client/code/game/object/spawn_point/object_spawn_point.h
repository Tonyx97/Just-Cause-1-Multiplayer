#pragma once

#include "spawn_point.h"

namespace jc::object_spawn_point
{
	static constexpr uint32_t FACTION = 0x12C; // Faction (uint32_t)

	namespace vt
	{
		static constexpr uint32_t SPAWN			 = 33;
		static constexpr uint32_t SPAWN_INTERNAL = 40;
	}
}

class ObjectSpawnPoint : public SpawnPoint
{
public:

	enum Faction : uint32_t
	{
		NoFaction = 0,
		Agency	  = 2,
		Military  = 3,
		Police	  = 4,
		Guerillas = 5,
		BlackHand = 6,
		Montano	  = 7,
		Rioja	  = 8
	};

	void set_faction(uint32_t faction);

	uint32_t get_faction() const;
};