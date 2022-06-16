#include <defs/standard.h>

#include "spawn_point.h"

void SpawnPoint::spawn_internal()
{
	jc::v_call(this, jc::spawn_point::vt::SPAWN_INTERNAL, true, true); // first bool = ignore limit
}

void SpawnPoint::spawn()
{
	jc::v_call(this, jc::spawn_point::vt::SPAWN);
}