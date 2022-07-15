#include <defs/standard.h>

#include "game_control.h"
#include "factory_system.h"

#include <game/transform/transform.h>
#include <game/sys/weapon_system.h>
#include <game/object/character/character.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/damageable_object/damageable_object.h>
#include <game/object/rigid_object/simple_rigid_object.h>
#include <game/object/rigid_object/animated_rigid_object.h>
#include <game/object/spawn_point/agent_spawn_point.h>
#include <game/object/spawn_point/vehicle_spawn_point.h>
#include <game/object/mounted_gun/mounted_gun.h>
#include <game/object/ladder/ladder.h>
#include <game/object/item/item_pickup.h>
#include <game/object/vars/exported_entities.h>
#include <game/object/mission/objective.h>
#include <game/object/ui/map_icon.h>

namespace jc::factory_system
{
	namespace v
	{
		vec<CharacterHandle*>			character_handles;
		vec<ref<SimpleRigidObject>>		simple_rigid_objects;
		vec<ref<DamageableObject>>		damageables;
		vec<ref<AgentSpawnPoint>>		agent_spawns;
		vec<ref<VehicleSpawnPoint>>		vehicle_spawns;
		vec<ref<MountedGun>>			mounted_guns;
		vec<ref<Ladder>>				ladders;
		vec<ref<ItemPickup>>			item_pickups;
		vec<ref<AnimatedRigidObject>>	animated_rigid_objects;
		vec<ref<UIMapIcon>>				ui_map_icons;
		vec<ref<Objective>>				objectives;
	}
}

using namespace jc::factory_system::v;

void FactorySystem::init()
{
	set_max_character_spawns(0);
	set_max_vehicle_spawns(0);
}

void FactorySystem::destroy()
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

void FactorySystem::set_max_character_spawns(int v)
{
	jc::write<int16_t>(v, this, jc::spawn_system::MAX_CHARACTER_SPAWNS);
}

void FactorySystem::set_max_vehicle_spawns(int v)
{
	jc::write<int16_t>(v, this, jc::spawn_system::MAX_VEHICLE_SPAWNS);
}

void FactorySystem::destroy_agent_spawn_point(AgentSpawnPoint* v)
{
	if (!v)
		return;

	agent_spawns.erase(std::remove_if(agent_spawns.begin(), agent_spawns.end(), [&](const auto& r) { return r.obj == v; }));
}

int16_t FactorySystem::get_max_character_spawns() const
{
	return jc::read<int16_t>(this, jc::spawn_system::MAX_CHARACTER_SPAWNS);
}

int16_t FactorySystem::get_max_vehicle_spawns() const
{
	return jc::read<int16_t>(this, jc::spawn_system::MAX_VEHICLE_SPAWNS);
}

CharacterHandle* FactorySystem::spawn_character(const std::string& model_name, const vec3& position, int weapon_id)
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
	{
		character_handles.push_back(handle);

		return handle;
	}

	return nullptr;
}

SimpleRigidObject* FactorySystem::spawn_simple_rigid_object(const vec3& position, const std::string& model_name, const std::string& pfx_name)
{
	Transform transform(position);

	if (auto rf = SimpleRigidObject::ALLOC()->create(&transform, model_name, pfx_name))
	{
		g_game_control->enable_object(rf);

		return rf.move_to(simple_rigid_objects);
	}

	return nullptr;
}

DamageableObject* FactorySystem::spawn_damageable_object(const vec3& position, const std::string& model_name, const std::string& pfx_name)
{
	Transform transform(position);

	if (auto rf = DamageableObject::CREATE(&transform, model_name, pfx_name))
	{
		g_game_control->enable_object(rf);

		return rf.move_to(damageables);
	}

	return nullptr;
}

AgentSpawnPoint* FactorySystem::create_agent_spawn_point(const vec3& position)
{
	if (auto rf = g_game_control->create_object<AgentSpawnPoint>())
	{
		object_base_map map {};

		map.insert<object_base_map::Int>(0xb77db56b, -1);
		map.insert<object_base_map::Int>(0xc37438ce, 3);
		map.insert<object_base_map::Int>(0xe2704360, -1);
		map.insert<object_base_map::Int>(0x5cc062db, 1);
		map.insert<object_base_map::Int>(0xd1b4133c, 0);
		map.insert<object_base_map::Int>(0xe9811540, 0);
		map.insert<object_base_map::Int>(0x422d64e6, 0);
		map.insert<object_base_map::Int>(0x4e870cc0, 0);
		map.insert<object_base_map::Int>(0xdcf5b98b, 0);
		map.insert<object_base_map::Int>(0x31386da9, 0);
		map.insert<object_base_map::Int>(0x31386da9, 0);
		map.insert<object_base_map::Int>(SpawnPoint::Hash_MaxHealth, 100);
		map.insert<object_base_map::Int>(ObjectBase::Hash_KeyObject, 0);
		map.insert<object_base_map::Int>(ObjectBase::Hash_Relative, 1);
		map.insert<object_base_map::Float>(ObjectBase::Hash_SpawnDistance, 40.f);
		map.insert<object_base_map::String>(0xdfe49873, R"(civilian\c_hooker.xml)");
		map.insert<object_base_map::String>(0xa4129310, R"(default_report_msg)");
		map.insert<object_base_map::String>(0x8eb5aff2, R"(exported\agenttypes\civilians\civ_bikinigirl2.ee)");
		map.insert<object_base_map::String>(0x51c3da11, R"(liberation_stop)");
		map.insert<object_base_map::String>(0x78ae93e7, R"(liberation_start)");
		map.insert<object_base_map::String>(0x7c5e1eae, R"(liberation_start)");
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(Beach Girl)");

		rf->init_from_map(&map);
		rf->set_position(position);

		return rf.move_to(agent_spawns);
	}

	return nullptr;
}

VehicleSpawnPoint* FactorySystem::create_vehicle_spawn_point(const vec3& position)
{
	if (auto rf = g_game_control->create_object<VehicleSpawnPoint>())
	{
		object_base_map map {};

		map.insert<object_base_map::Int>(0x3f23e8c2, 0);
		map.insert<object_base_map::Int>(0x26fd83ce, 1);
		map.insert<object_base_map::Int>(0xf2bf8625, 30);
		map.insert<object_base_map::Int>(0x604e298b, 0);
		map.insert<object_base_map::Int>(0x5f2bb3f6, 0);
		map.insert<object_base_map::Int>(0x35432b1a, 0);
		map.insert<object_base_map::Int>(0x391fc12, 0);
		map.insert<object_base_map::Int>(0x4b5abaf, 0);
		map.insert<object_base_map::Int>(0xe2704360, 0);
		map.insert<object_base_map::Int>(0x31386da9, 0);
		map.insert<object_base_map::Int>(0xc66ca195, 0);
		map.insert<object_base_map::Int>(0x31386da9, 0);
		map.insert<object_base_map::Int>(SpawnPoint::Hash_MaxHealth, 100);
		map.insert<object_base_map::Int>(ObjectBase::Hash_KeyObject, 1);
		map.insert<object_base_map::Float>(ObjectBase::Hash_SpawnDistance, 150.f);
		map.insert<object_base_map::String>(0x7c87224d, R"(rm_respawn rm_11_release)");
		map.insert<object_base_map::String>(0x6ea4cb25, R"(rm_11_release)");
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(rm_11_vehicle)");
		map.insert<object_base_map::String>(0x8eb5aff2, jc::vars::exported_entities_vehicles[7]);
		// map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, {});

		log(RED, "VehicleSpawnPoint: {:x}", ptr(*rf));

		rf->init_from_map(&map);
		rf->set_position(position);
		rf->set_faction(VehFaction_BlackHand);

		return rf.move_to(vehicle_spawns);
	}

	return nullptr;
}

MountedGun* FactorySystem::spawn_mounted_gun(const vec3& position)
{
	if (auto rf = g_game_control->create_object<MountedGun>())
	{
		Transform transform(position);

		object_base_map map {};

		map.insert<object_base_map::Int>(0x66869321, 1);
		map.insert<object_base_map::Float>(0x71b9da94, -6.25f);
		map.insert<object_base_map::Float>(0x9fcd3bc1, 1.f);
		map.insert<object_base_map::Float>(0x9b92cd11, -6.33f);
		map.insert<object_base_map::Float>(0x29ae9b9d, 0.48f);
		map.insert<object_base_map::Float>(0x8515daf3, 8.f);
		map.insert<object_base_map::Float>(0x9c6cd67c, 8.f);
		map.insert<object_base_map::Float>(0xed737006, 0.07f);
		map.insert<object_base_map::Float>(0x3b6ca735, 0.09f);
		map.insert<object_base_map::Float>(0xfa10a8d, 100.f);
		map.insert<object_base_map::Float>(0xfff34f1e, 0.70f);
		map.insert<object_base_map::Float>(0xf4787d50, 1.f);
		map.insert<object_base_map::String>(0x274e57fb, "Mounted M60");
		//map.insert<object_base_map::String>(0xA4AB5487, "weapons\\weap_011_lave_037_mount.lod");
		//map.insert<object_base_map::String>(0x3921ad5f, "weapons\\weap_011_lave_037_mount.lod");
		map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, &transform);
		//map.insert<object_base_map::Mat4>(0x316ec15e, .); // put your matrix here
		//map.insert<object_base_map::Mat4>(0xb5be7095, .); // put your matrix here

		rf->init_from_map(&map);

		return rf.move_to(mounted_guns);
	}

	return nullptr;
}

Ladder* FactorySystem::spawn_ladder(const vec3& position, const std::string& model, float length)
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
		map.insert<object_base_map::Int>(0x2c9331bd, 1);
		map.insert<object_base_map::Float>(0x14936868, 0.6f);
		map.insert<object_base_map::String>(ObjectBase::Hash_PFX, (model + ".pfx").c_str());
		map.insert<object_base_map::String>(ObjectBase::Hash_LOD_Model, (model + ".lod").c_str());
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, "CLadder1");
		map.insert<object_base_map::String>(ObjectBase::Hash_Class, "CLadder");
		map.insert<object_base_map::Vec3>(0xc2292175, &offset);
		map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, &transform.get_matrix());
		map.insert<object_base_map::Mat4>(0x526d67dc, &info);

		rf->init_from_map(&map);

		return rf.move_to(ladders);
	}

	return nullptr;
}

ItemPickup* FactorySystem::spawn_general_item_pickup(const vec3& position, uint32_t type, const std::string& model, const std::string& description)
{
	if (auto rf = g_game_control->create_object<ItemPickup>())
	{
		if (!rf->setup(position, type, Weapon_None, model, description))
			return nullptr;

		return rf.move_to(item_pickups);
	}

	return nullptr;
}

ItemPickup* FactorySystem::spawn_weapon_item_pickup(const vec3& position, uint32_t weapon_id, const std::string& description)
{
	if (auto rf = g_game_control->create_object<ItemPickup>())
	{
		if (!rf->setup(position, ItemType_Weapon, weapon_id, {}, description))
			return nullptr;

		return rf.move_to(item_pickups);
	}

	return nullptr;
}

AnimatedRigidObject* FactorySystem::spawn_animated_rigid_object(const vec3& position, const std::string& model, const std::string& pfx_name)
{
	if (auto rf = g_game_control->create_object<AnimatedRigidObject>())
	{
		object_base_map map {};

		Transform transform(position);

		const auto m0 = mat4 { -1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.56f, 1.f, 0.f, 1.f };

		map.insert<object_base_map::Int>(0x2c9331bd, 1); // int
		map.insert<object_base_map::Int>(0x26299a20, 1); // int
		map.insert<object_base_map::Int>(0x65965327, 0); // int
		map.insert<object_base_map::Int>(0x52702583, 0); // int
		map.insert<object_base_map::Int>(0x525a07d4, 0); // int
		map.insert<object_base_map::Int>(0x8597f162, 0); // int
		map.insert<object_base_map::Int>(0x78b67171, 1); // int
		map.insert<object_base_map::Int>(0xa1f2b8b1, 0); // int
		map.insert<object_base_map::Int>(0xe7916975, 0); // int
		map.insert<object_base_map::Int>(0xda3feaea, 1); // int
		map.insert<object_base_map::Float>(0x49985996, 0.f); // float
		map.insert<object_base_map::Float>(0x1d39cbef, 0.5f); // float
		map.insert<object_base_map::Float>(0x2dd7ffe, 0.f); // float
		map.insert<object_base_map::Float>(0xb33b958d, 0.f); // float
		map.insert<object_base_map::Float>(0xc868ac91, 0.f); // float
		map.insert<object_base_map::String>(ObjectBase::Hash_LOD_Model, model); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_PFX, pfx_name); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(CAnimatedRigidObject1)"); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_Class, R"(CAnimatedRigidObject)"); // string
		map.insert<object_base_map::String>(0xf83e4d54, R"(building_blocks\animations\z_90.anim)"); // string
		//map.insert<object_base_map::String>(0x355f55e0, R"(custom::anim_end)"); // string
		//map.insert<object_base_map::String>(0xca2ea3a9, R"(custom::opendoor)"); // string
		//map.insert<object_base_map::String>(0xae7c2d5e, R"(custom::closedoor)"); // string
		map.insert<object_base_map::Mat4>(0x72f35d2, &m0); // mat4
		map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, &transform); // mat4

		rf->init_from_map(&map);

		return rf.move_to(animated_rigid_objects);
	}

	return nullptr;
}

UIMapIcon* FactorySystem::create_map_icon(const vec3& position, uint32_t icon)
{
	if (auto rf = g_game_control->create_object<UIMapIcon>(false))
	{
		if (!rf->setup(position, icon))
			return nullptr;

		return rf.move_to(ui_map_icons);
	}

	return nullptr;
}

Objective* FactorySystem::create_objective(const vec3& position, const u8vec4& color)
{
	if (auto rf = g_game_control->create_object<Objective>(false))
	{
		if (!rf->setup(position, color))
			return nullptr;

		return rf.move_to(objectives);
	}

	return nullptr;
}