#pragma once

#include "../base/base.h"

namespace jc::item_pickup
{
	static constexpr uint32_t RESPAWN_TIME			= 0x7C;
	static constexpr uint32_t RESPAWN_TIME_LEFT		= 0xC0;

	namespace fn
	{
		static constexpr uint32_t CREATE = 0x6661C0;
	}

	namespace hook
	{
		//using on_item_pickup_t = prototype<void(__thiscall*)(void*, int*, ptr, std::string*, bool, float), 0x77CE20, util::hash::JENKINS("OnItemPickup")>;

		void apply();
		void undo();
	}
}

enum ItemType : uint32_t
{
	ItemType_Health,
	ItemType_Weapon,
	ItemType_Ammo,
	ItemType_Grenade,
	ItemType_Collectible
};

class ItemPickup : public ObjectBase
{
public:

	IMPL_OBJECT_TYPE_ID("CItemPickup");

	ItemPickup();
	~ItemPickup();

	bool setup(const Transform& transform, uint32_t type, uint32_t weapon_id, const std::string& model = {}, const std::string& description = {});

	void set_respawn_time(float v);
	void set_respawn_time_left(float v);

	float get_respawn_time() const;
	float get_respawn_time_left() const;
};