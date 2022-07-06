#pragma once

#include "../base/base.h"

namespace jc::item_pickup
{

	static constexpr uint32_t RESPAWN_TIME = 0xC0;

	namespace fn
	{
		static constexpr uint32_t CREATE = 0x6661C0;
	}
}

class ItemPickup : public ObjectBase
{
public:

	IMPL_OBJECT_TYPE_ID("CItemPickup");

	ItemPickup();
	~ItemPickup();

	void set_respawn_time(float v);
};