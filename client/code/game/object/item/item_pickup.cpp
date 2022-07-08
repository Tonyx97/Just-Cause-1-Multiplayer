#include <defs/standard.h>

#include "item_pickup.h"

ItemPickup::ItemPickup()
{
}

ItemPickup::~ItemPickup()
{
}

void ItemPickup::set_respawn_time(float v)
{
	jc::write(jc::game::float_to_i16(v), this, jc::item_pickup::RESPAWN_TIME);
}