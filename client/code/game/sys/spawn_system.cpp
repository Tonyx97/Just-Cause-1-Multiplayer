#include <defs/standard.h>

#include "game_control.h"
#include "spawn_system.h"

#include <game/transform/transform.h>
#include <game/sys/weapon_system.h>
#include <game/object/character/character.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/damageable_object/damageable_object.h>
#include <game/object/rigid_object/simple_rigid_object.h>
#include <game/object/spawn_point/agent_spawn_point.h>
#include <game/object/spawn_point/vehicle_spawn_point.h>
#include <game/object/mounted_gun/mounted_gun.h>
#include <game/object/ladder/ladder.h>
#include <game/object/item/item_pickup.h>

namespace jc::spawn_system
{
	namespace v
	{
		vec<CharacterHandle*>		character_handles;
		vec<ref<SimpleRigidObject>> simple_rigid_objects;
		vec<ref<DamageableObject>>	damageables;
		vec<ref<AgentSpawnPoint>>	agent_spawns;
		vec<ref<VehicleSpawnPoint>> vehicle_spawns;
		vec<ref<MountedGun>>		mounted_guns;
		vec<ref<Ladder>>			ladders;
		vec<ref<ItemPickup>>		item_pickups;

		template <typename T>
		T* add_item(auto& container, ref<T>& r)
		{
			const auto item = *r;

			container.push_back(std::move(r));

			return item;
		}

		template <typename T>
		T* add_item_nr(auto& container, T* r)
		{
			container.push_back(r);

			return r;
		}
	}
}

using namespace jc::spawn_system::v;

void SpawnSystem::init()
{
	// set_max_character_spawns(0);
	// set_max_vehicle_spawns(0);
}

void SpawnSystem::destroy()
{
	// clear our containers

	for (auto handle : character_handles)
		handle->destroy();

	character_handles.clear();
	simple_rigid_objects.clear();
	damageables.clear();
	agent_spawns.clear();
	vehicle_spawns.clear();

	// set the default amount of spawns for characters and vehicles

	set_max_character_spawns(20);
	set_max_vehicle_spawns(20);
}

void SpawnSystem::set_max_character_spawns(int v)
{
	jc::write<int16_t>(v, this, jc::spawn_system::MAX_CHARACTER_SPAWNS);
}

void SpawnSystem::set_max_vehicle_spawns(int v)
{
	jc::write<int16_t>(v, this, jc::spawn_system::MAX_VEHICLE_SPAWNS);
}

void SpawnSystem::destroy_agent_spawn_point(AgentSpawnPoint* v)
{
	if (!v)
		return;

	agent_spawns.erase(std::remove_if(agent_spawns.begin(), agent_spawns.end(), [&](const auto& r) { return r.obj == v; }));
}

int16_t SpawnSystem::get_max_character_spawns() const
{
	return jc::read<int16_t>(this, jc::spawn_system::MAX_CHARACTER_SPAWNS);
}

int16_t SpawnSystem::get_max_vehicle_spawns() const
{
	return jc::read<int16_t>(this, jc::spawn_system::MAX_VEHICLE_SPAWNS);
}

CharacterHandle* SpawnSystem::spawn_character(const std::string& model_name, const vec3& position, int weapon_id)
{
	CharacterInfo info {};

	info.is_female			= 0;
	info.y					= 0.4f;
	info.z					= 1;
	info.w					= 50.f;
	info.name				= "JCMP_CHARACTER";
	info.model				= "characters\\paperdolls\\paperdoll_" + model_name + ".lod";
	info.eyelid				= "characters\\kc_001\\kc_001_eyelid.rbm";
	info.skeleton			= "models\\characters\\kc_001\\kc_001.skl";
	info.character_as		= "settings\\animationsets\\characterrico.as";
	info.upper_body_as		= "settings\\animationsets\\upperbodyrico.as";
	info.base_character_as	= "settings\\animationsets\\basecharacterrico.as";
	info.base_upper_body_as = "settings\\animatiosSets\\baseupperbodyrico.as";

	Transform transform(position);

	if (const auto handle = CharacterHandle::GET_FREE_HANDLE()->create(&info, &transform, weapon_id))
		return add_item_nr(character_handles, handle);

	return nullptr;
}

SimpleRigidObject* SpawnSystem::spawn_simple_rigid_object(const vec3& position, const std::string& model_name, const std::string& pfx_name)
{
	Transform transform(position);

	if (auto rf = SimpleRigidObject::ALLOC()->create(&transform, model_name, pfx_name))
	{
		g_game_control->enable_object(rf);

		return add_item(simple_rigid_objects, rf);
	}

	return nullptr;
}

DamageableObject* SpawnSystem::spawn_damageable_object(const vec3& position, const std::string& model_name, const std::string& pfx_name)
{
	Transform transform(position);

	if (auto rf = DamageableObject::CREATE(&transform, model_name, pfx_name))
	{
		g_game_control->enable_object(rf);

		return add_item(damageables, rf);
	}

	return nullptr;
}

AgentSpawnPoint* SpawnSystem::create_agent_spawn_point(const vec3& position)
{
	if (auto rf = g_game_control->create_object<AgentSpawnPoint>())
	{
		object_base_map map {};

		map.insert<object_base_map::String>(0xdfe49873, R"(civilian\c_hooker.xml)");
		map.insert<object_base_map::Int>(0xb77db56b, -1);
		map.insert<object_base_map::Int>(0xc37438ce, 3);
		map.insert<object_base_map::Int>(0xe2704360, -1);
		map.insert<object_base_map::Int>(0x5cc062db, 1);
		map.insert<object_base_map::Int>(0xd1b4133c, 0);
		map.insert<object_base_map::Int>(0xe9811540, 0);
		map.insert<object_base_map::Int>(0x422d64e6, 0);
		map.insert<object_base_map::Int>(0x4e870cc0, 0);
		map.insert<object_base_map::Int>(HASH_MAX_HEALTH, 100);
		map.insert<object_base_map::Int>(0xdcf5b98b, 0);
		map.insert<object_base_map::String>(HASH_DESCRIPTION, R"(Beach Girl)");
		map.insert<object_base_map::Int>(0x31386da9, 0);
		map.insert<object_base_map::String>(0xa4129310, R"(default_report_msg)");
		map.insert<object_base_map::String>(0x8eb5aff2, R"(exported\agenttypes\civilians\civ_bikinigirl2.ee)");
		map.insert<object_base_map::Int>(HASH_KEY_OBJECT, 0);
		map.insert<object_base_map::String>(0x51c3da11, R"(liberation_stop)");
		map.insert<object_base_map::String>(0x78ae93e7, R"(liberation_start)");
		map.insert<object_base_map::String>(0x7c5e1eae, R"(liberation_start)");
		map.insert<object_base_map::Int>(HASH_RELATIVE, 1);
		map.insert<object_base_map::Float>(HASH_SPAWN_DISTANCE, 40.00f);
		map.insert<object_base_map::Int>(0x31386da9, 0);

		rf->init_from_map(&map);
		rf->set_position(position);

		return add_item(agent_spawns, rf);
	}

	return nullptr;
}

VehicleSpawnPoint* SpawnSystem::create_vehicle_spawn_point(const vec3& position)
{
	if (auto rf = g_game_control->create_object<VehicleSpawnPoint>())
	{
		object_base_map map {};

		map.insert<object_base_map::Int>(0x3f23e8c2, 0);
		map.insert<object_base_map::Int>(0x26fd83ce, 1);
		map.insert<object_base_map::Int>(0xf2bf8625, 30);
		map.insert<object_base_map::String>(0x7c87224d, R"(rm_respawn rm_11_release)");
		map.insert<object_base_map::String>(0x6ea4cb25, R"(rm_11_release)");
		map.insert<object_base_map::Int>(0x604e298b, 0);
		map.insert<object_base_map::Int>(0x5f2bb3f6, 0);
		map.insert<object_base_map::Int>(0x35432b1a, 0);
		map.insert<object_base_map::Int>(0x391fc12, 0);
		map.insert<object_base_map::Int>(0x4b5abaf, 0);
		map.insert<object_base_map::Int>(0xe2704360, 0);
		// map.insert<object_base_map::Mat4>(HASH_TRANSFORM, {}); // put your matrix here
		map.insert<object_base_map::String>(HASH_DESCRIPTION, R"(rm_11_vehicle)");
		map.insert<object_base_map::Int>(0x31386da9, 0);
		map.insert<object_base_map::String>(0x8eb5aff2, R"(Exported\Vehicles\Land_Vehicles\LAVE_031_Pickup_4_BMG.ee)");
		map.insert<object_base_map::Int>(HASH_KEY_OBJECT, 1);
		map.insert<object_base_map::Int>(0xc66ca195, 0);
		map.insert<object_base_map::Float>(HASH_SPAWN_DISTANCE, 150.00f);
		map.insert<object_base_map::Int>(0x31386da9, 0);

		log(RED, "VehicleSpawnPoint: {:x}", ptr(*rf));

		rf->init_from_map(&map);
		rf->set_position(position);
		rf->set_faction(VehicleSpawnPoint::BlackHand);

		return add_item(vehicle_spawns, rf);
	}

	return nullptr;
}

MountedGun* SpawnSystem::spawn_mounted_gun(const vec3& position)
{
	if (auto rf = g_game_control->create_object<MountedGun>())
	{
		Transform transform(position);

		object_base_map map {};

		map.insert<object_base_map::String>(0x274e57fb, "Mounted M60");
		map.insert<object_base_map::Float>(0x71b9da94, -6.25f);
		map.insert<object_base_map::Float>(0x9fcd3bc1, 1.00f);
		map.insert<object_base_map::Float>(0x9b92cd11, -6.33f);
		map.insert<object_base_map::Float>(0x29ae9b9d, 0.48f);
		map.insert<object_base_map::Float>(0x8515daf3, 8.00f);
		map.insert<object_base_map::Float>(0x9c6cd67c, 8.00f);
		map.insert<object_base_map::Float>(0xed737006, 0.07f);
		map.insert<object_base_map::Float>(0x3b6ca735, 0.09f);
		map.insert<object_base_map::Float>(0xfa10a8d, 100.00f);
		//map.insert<object_base_map::Mat4>(0x316ec15e, .); // put your matrix here
		//map.insert<object_base_map::Mat4>(0xb5be7095, .); // put your matrix here
		map.insert<object_base_map::Float>(0xfff34f1e, 0.70f);
		map.insert<object_base_map::Float>(0xf4787d50, 1.00f);
		map.insert<object_base_map::Int>(0x66869321, 1);
		map.insert<object_base_map::Mat4>(HASH_TRANSFORM, &transform);
		//map.insert<object_base_map::String>(0xA4AB5487, "weapons\\weap_011_lave_037_mount.lod");
		//map.insert<object_base_map::String>(0x3921ad5f, "weapons\\weap_011_lave_037_mount.lod");

		rf->init_from_map(&map);

		g_game_control->enable_object(rf);

		return add_item(mounted_guns, rf);
	}

	return nullptr;
}

Ladder* SpawnSystem::spawn_ladder(const vec3& position, const std::string& model, float length)
{
	if (auto rf = g_game_control->create_object<Ladder>())
	{
		object_base_map map {};

		mat4 info
		{
			0.f, 0.f, 0.f, 0.f,
			0.f, length, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f
		};

		Transform transform(position);

		vec3 offset { -1.f, 0.f, 0.f };

		map.insert<object_base_map::Int>(0x525a07d4, 0);
		map.insert<object_base_map::Float>(0x14936868, 0.6f);
		map.insert<object_base_map::Mat4>(HASH_TRANSFORM, &transform.get_matrix());
		map.insert<object_base_map::Mat4>(0x526d67dc, &info);
		map.insert<object_base_map::Int>(0x2c9331bd, 1);
		map.insert<object_base_map::String>(0x5b982501, (model + ".pfx").c_str());
		map.insert<object_base_map::String>(0xea402acf, (model + ".lod").c_str());
		map.insert<object_base_map::String>(HASH_DESCRIPTION, "CLadder1");
		map.insert<object_base_map::String>(HASH_CLASS, "CLadder");
		map.insert<object_base_map::Vec3>(0xc2292175, &offset);

		rf->init_from_map(&map);

		return add_item(ladders, rf);
	}

	return nullptr;
}

ItemPickup* SpawnSystem::spawn_general_item_pickup(const vec3& position, uint32_t type, const std::string& model, const std::string& description)
{
	if (auto rf = g_game_control->create_object<ItemPickup>())
	{
		if (!rf->setup(position, type, Weapon_None, model, description))
			return nullptr;

		return add_item(item_pickups, rf);
	}

	return nullptr;
}

ItemPickup* SpawnSystem::spawn_weapon_item_pickup(const vec3& position, uint32_t weapon_id, const std::string& description)
{
	if (auto rf = g_game_control->create_object<ItemPickup>())
	{
		if (!rf->setup(position, ItemType_Weapon, weapon_id, {}, description))
			return nullptr;

		return add_item(item_pickups, rf);
	}

	return nullptr;
}