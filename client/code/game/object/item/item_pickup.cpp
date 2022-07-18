#include <defs/standard.h>

#include "item_pickup.h"

#include <game/sys/weapon_system.h>

namespace jc::item_pickup::hook
{
	void apply()
	{
	}

	void undo()
	{
	}
}

ItemPickup::ItemPickup()
{
}

ItemPickup::~ItemPickup()
{
}

bool ItemPickup::setup(const Transform& transform, uint32_t type, uint32_t weapon_id, const std::string& model, const std::string& description)
{
	object_base_map map {};

	switch (type)
	{
	case ItemType_Health:
	case ItemType_Ammo:
	case ItemType_Grenade:
	case ItemType_Collectible:
	{
		map.insert<object_base_map::Int>(0xc6fc5f96, 1); // int
		map.insert<object_base_map::String>(0xc4c33843, model); // string

		break;
	}
	case ItemType_Weapon:
	{
		if (const auto weapon_model = g_weapon->get_weapon_model(weapon_id); !weapon_model.empty())
		{
			map.insert<object_base_map::Int>(0xc6fc5f96, 0); // int
			map.insert<object_base_map::Int>(0x773aff1c, 1); // int
			map.insert<object_base_map::String>(0xc4c33843, weapon_model); // string
			map.insert<object_base_map::String>(0xec164522, g_weapon->get_weapon_typename(weapon_id)); // string

			if (weapon_id == Weapon_Triggered_Explosive || weapon_id == Weapon_Timed_Explosive)
				map.insert<object_base_map::String>(0x3d82d7e, weapon_model); // string

			break;
		}

		log(RED, "Weapon id {} is invalid", weapon_id);

		return false;
	}
	}

	if (!description.empty())
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, description); // string

	map.insert<object_base_map::Int>(0x7580ba87, 0); // int
	map.insert<object_base_map::Int>(0x3f554d9b, 0); // int
	map.insert<object_base_map::Int>(0x6261032, 1); // int
	map.insert<object_base_map::Int>(0x43f66557, 0); // int
	map.insert<object_base_map::Int>(0x42f184ea, 1); // int
	map.insert<object_base_map::Int>(0x525a07d4, 0); // int
	map.insert<object_base_map::Int>(0x9e3d7878, 0); // int
	map.insert<object_base_map::Int>(0x9098c79d, 0); // int
	map.insert<object_base_map::Int>(0x9a9d9a7f, 1); // int
	map.insert<object_base_map::Int>(0xaae1437b, type); // int
	map.insert<object_base_map::Int>(0xee2cc81d, 1); // int
	map.insert<object_base_map::Float>(0xce44e7b2, 2.5f); // float - respawn time
	map.insert<object_base_map::Float>(0xd6c4e1ec, 0.f); // float
	map.insert<object_base_map::String>(0xdb33b0ba, "Item"); // string
	map.insert<object_base_map::String>(ObjectBase::Hash_Class, R"(CItemPickup)"); // string
	map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, &transform); // mat4

	init_from_map(&map);
	set_respawn_time_left(0.f);

	return true;
}

void ItemPickup::set_respawn_time(float v)
{
	jc::write(jc::game::float_to_i16(v), this, jc::item_pickup::RESPAWN_TIME);
}

void ItemPickup::set_respawn_time_left(float v)
{
	jc::write(jc::game::float_to_i16(v), this, jc::item_pickup::RESPAWN_TIME_LEFT);
}

float ItemPickup::get_respawn_time() const
{
	return jc::game::i16_to_float(jc::read<uint16_t>(this, jc::item_pickup::RESPAWN_TIME));
}

float ItemPickup::get_respawn_time_left() const
{
	return jc::game::i16_to_float(jc::read<uint16_t>(this, jc::item_pickup::RESPAWN_TIME_LEFT));
}