#include <defs/standard.h>

#include "object_spawn_point.h"

void ObjectSpawnPoint::set_faction(uint32_t faction)
{
	jc::write(faction, this, jc::object_spawn_point::FACTION);
}

uint32_t ObjectSpawnPoint::get_faction() const
{
	return jc::read<uint32_t>(this, jc::object_spawn_point::FACTION);
}