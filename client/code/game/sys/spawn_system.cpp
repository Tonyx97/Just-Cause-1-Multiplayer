#include <defs/standard.h>

#include "game_control.h"
#include "spawn_system.h"

#include <game/object/character/character.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/damageable_object/damageable_object.h>
#include <game/object/simple_rigid_object/simple_rigid_object.h>
#include <game/object/transform/transform.h>
#include <game/object/spawn_point/agent_spawn_point.h>
#include <game/object/spawn_point/vehicle_spawn_point.h>
#include <game/object/mounted_gun/mounted_gun.h>

namespace jc::spawn_system
{
	namespace v
	{
		vec<ref<SimpleRigidObject>> simple_rigid_objects;
		vec<ref<DamageableObject>>	damageables;
		vec<ref<AgentSpawnPoint>>	agent_spawns;
		vec<ref<VehicleSpawnPoint>> vehicle_spawns;
		vec<ref<MountedGun>>		mounted_guns;
	}
}

void SpawnSystem::init()
{
	// set_max_character_spawns(0);
	// set_max_vehicle_spawns(0);
}

void SpawnSystem::destroy()
{
	using namespace jc::spawn_system::v;

	// clear our containers

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

	info.x					= 0.f;
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

	return CharacterHandle::GET_FREE_HANDLE()->create(&info, &transform, weapon_id);
}

SimpleRigidObject* SpawnSystem::spawn_simple_rigid_object(const vec3& position, const std::string& model_name, const std::string& pfx_name)
{
	Transform transform(position);

	if (auto rf = SimpleRigidObject::ALLOC()->create(&transform, model_name, pfx_name))
	{
		g_game_control->enable_object(rf);

		jc::spawn_system::v::simple_rigid_objects.push_back(std::move(rf));

		return *rf;
	}

	return nullptr;
}

DamageableObject* SpawnSystem::spawn_damageable_object(const vec3& position, const std::string& model_name, const std::string& pfx_name)
{
	Transform transform(position);

	if (auto rf = DamageableObject::CREATE(&transform, model_name, pfx_name))
	{
		g_game_control->enable_object(rf);

		jc::spawn_system::v::damageables.push_back(std::move(rf));

		return *rf;
	}

	return nullptr;
}

AgentSpawnPoint* SpawnSystem::create_agent_spawn_point(const vec3& position)
{
	if (auto rf = g_game_control->create_object<AgentSpawnPoint>())
	{
		object_base_map map {};

		map.insert<ValueType_String>(0xdfe49873, R"(civilian\c_hooker.xml)");
		map.insert<ValueType_Int>(0xb77db56b, -1);
		map.insert<ValueType_Int>(0xc37438ce, 3);
		map.insert<ValueType_Int>(0xe2704360, -1);
		map.insert<ValueType_Bool>(0x5cc062db, true);
		map.insert<ValueType_Bool>(0xd1b4133c, false);
		map.insert<ValueType_Bool>(0xe9811540, false);
		map.insert<ValueType_Bool>(0x422d64e6, false);
		map.insert<ValueType_Bool>(0x4e870cc0, false);
		map.insert<ValueType_Int>(0xe1f06be0, 100);
		map.insert<ValueType_Bool>(0xdcf5b98b, false);
		map.insert<ValueType_String>(0xb8fbd88e, R"(Beach Girl)");
		map.insert<ValueType_Bool>(0x31386da9, false);
		map.insert<ValueType_String>(0xa4129310, R"(default_report_msg)");
		map.insert<ValueType_String>(0x8eb5aff2, R"(exported\agenttypes\civilians\civ_bikinigirl2.ee)");
		map.insert<ValueType_Int>(0x69a3a614, 0);
		map.insert<ValueType_String>(0x51c3da11, R"(liberation_stop)");
		map.insert<ValueType_String>(0x78ae93e7, R"(liberation_start)");
		map.insert<ValueType_String>(0x7c5e1eae, R"(liberation_start)");
		map.insert<ValueType_Int>(0x773aff1c, 1);
		map.insert<ValueType_Float>(0xd2f9579a, 40.00f);
		map.insert<ValueType_Int>(0x31386da9, 0);

		const auto spawn_point = *rf;

		spawn_point->init_from_map(&map);
		spawn_point->set_position(position);

		g_game_control->enable_object(rf);

		jc::spawn_system::v::agent_spawns.push_back(std::move(rf));

		return spawn_point;
	}

	return nullptr;
}

VehicleSpawnPoint* SpawnSystem::create_vehicle_spawn_point(const vec3& position)
{
	if (auto rf = g_game_control->create_object<VehicleSpawnPoint>())
	{
		object_base_map map {};

		map.insert<ValueType_Int>(0x3f23e8c2, 0);
		map.insert<ValueType_Int>(0x26fd83ce, 1);
		map.insert<ValueType_Int>(0xf2bf8625, 30);
		map.insert<ValueType_String>(0x7c87224d, R"(rm_respawn rm_11_release)");
		map.insert<ValueType_String>(0x6ea4cb25, R"(rm_11_release)");
		map.insert<ValueType_Int>(0x604e298b, 0);
		map.insert<ValueType_Int>(0x5f2bb3f6, 0);
		map.insert<ValueType_Int>(0x35432b1a, 0);
		map.insert<ValueType_Int>(0x391fc12, 0);
		map.insert<ValueType_Int>(0x4b5abaf, 0);
		map.insert<ValueType_Int>(0xe2704360, 0);
		// map.insert<ValueType_Mat4>(0xacaefb1, {}); // put your matrix here
		map.insert<ValueType_String>(0xb8fbd88e, R"(rm_11_vehicle)");
		map.insert<ValueType_Bool>(0x31386da9, false);
		map.insert<ValueType_String>(0x8eb5aff2, R"(Exported\Vehicles\Land_Vehicles\LAVE_031_Pickup_4_BMG.ee)");
		map.insert<ValueType_Int>(0x69a3a614, 1);
		map.insert<ValueType_Int>(0xc66ca195, 0);
		map.insert<ValueType_Float>(0xd2f9579a, 150.00f);
		map.insert<ValueType_Int>(0x31386da9, 0);

		const auto spawn_point = *rf;

		log(RED, "VehicleSpawnPoint: {:x}", ptr(*rf));

		spawn_point->init_from_map(&map);
		spawn_point->set_position(position);
		spawn_point->set_faction(VehicleSpawnPoint::BlackHand);

		g_game_control->enable_object(rf);

		jc::spawn_system::v::vehicle_spawns.push_back(std::move(rf));

		return spawn_point;
	}

	return nullptr;
}

MountedGun* SpawnSystem::spawn_mounted_gun(const vec3& position)
{
	if (auto rf = g_game_control->create_object<MountedGun>())
	{
		Transform transform(position);

		object_base_map map {};

		map.insert<ValueType_String>(0x274e57fb, R"(Mounted M60)");
		map.insert<ValueType_Float>(0x71b9da94, -6.25f);
		map.insert<ValueType_Float>(0x9fcd3bc1, 1.00f);
		map.insert<ValueType_Float>(0x9b92cd11, -6.33f);
		map.insert<ValueType_Float>(0x29ae9b9d, 0.48f);
		map.insert<ValueType_Float>(0x8515daf3, 8.00f);
		map.insert<ValueType_Float>(0x9c6cd67c, 8.00f);
		map.insert<ValueType_Float>(0xed737006, 0.07f);
		map.insert<ValueType_Float>(0x3b6ca735, 0.09f);
		map.insert<ValueType_Float>(0xfa10a8d, 100.00f);
		//map.insert<ValueType_Mat4>(0x316ec15e, .); // put your matrix here
		//map.insert<ValueType_Mat4>(0xb5be7095, .); // put your matrix here
		map.insert<ValueType_Float>(0xfff34f1e, 0.70f);
		map.insert<ValueType_Float>(0xf4787d50, 1.00f);
		map.insert<ValueType_Bool>(0x66869321, true);
		map.insert<ValueType_Mat4>(0xACAEFB1, &transform);
		//map.insert<ValueType_String>(0xA4AB5487, "weapons\\weap_011_lave_037_mount.lod");
		//map.insert<ValueType_String>(0x3921ad5f, "weapons\\weap_011_lave_037_mount.lod");

		const auto mounted_gun = *rf;

		mounted_gun->init_from_map(&map);

		g_game_control->enable_object(rf);

		jc::spawn_system::v::mounted_guns.push_back(std::move(rf));

		return mounted_gun;
	}

	return nullptr;
}