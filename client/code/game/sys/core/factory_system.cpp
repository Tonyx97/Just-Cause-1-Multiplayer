#include <defs/standard.h>
#include <defs/client_basic.h>

#include "../game/game_control.h"

#include "factory_system.h"

#include <game/transform/transform.h>
#include <game/sys/weapon/weapon_system.h>
#include <game/sys/resource/resource_streamer.h>
#include <game/object/character/character.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/damageable_object/damageable_object.h>
#include <game/object/damageable_object/grenade.h>
#include <game/object/rigid_object/simple_rigid_object.h>
#include <game/object/rigid_object/animated_rigid_object.h>
#include <game/object/rigid_object/traffic_light.h>
#include <game/object/spawn_point/agent_spawn_point.h>
#include <game/object/spawn_point/vehicle_spawn_point.h>
#include <game/object/mounted_gun/mounted_gun.h>
#include <game/object/agent_type/vehicle_type.h>
#include <game/object/ladder/ladder.h>
#include <game/object/item/item_pickup.h>
#include <game/object/vars/exported_entities.h>
#include <game/object/mission/objective.h>
#include <game/object/ui/map_icon.h>
#include <game/object/ui/map_icon_type.h>
#include <game/object/sound/sound_game_obj.h>
#include <game/object/game_resource/ee_resource.h>
#include <game/object/exported_entity/exported_entity.h>
#include <game/object/vehicle/vehicle.h>
#include <game/sys/resource/archives.h>

namespace jc::factory_system
{
	namespace v
	{
		std::unordered_set<CharacterHandle*>		character_handles;

		ref_map<SimpleRigidObject>					simple_rigid_objects;
		ref_map<DamageableObject>					damageables;
		ref_map<Vehicle>							vehicles;
		ref_map<AgentSpawnPoint>					agent_spawns;
		ref_map<VehicleSpawnPoint>					vehicle_spawns;
		ref_map<MountedGun>							mounted_guns;
		ref_map<Ladder>								ladders;
		ref_map<ItemPickup>							item_pickups;
		ref_map<AnimatedRigidObject>				animated_rigid_objects;
		ref_map<UIMapIcon>							ui_map_icons;
		ref_map<UIMapIconType>						ui_map_icon_types;
		ref_map<Objective>							objectives;
		ref_map<TrafficLight>						traffic_lights;
		ref_map<SoundGameObject>					sounds;

		template <typename T, typename V>
		auto add_to_ref_map(T& t, V& v)
		{
			const auto value = v.get();

			t.insert({ v.get(), v });

			return value;
		}
	}
}

using namespace jc::factory_system::v;

template <typename T>
T read_ee(auto ptr, size_t& offset)
{
	return *(T*)(ptr + std::exchange(offset, offset + sizeof(T)));
}

void read_ee_mem(auto ptr, size_t& offset, void* out, size_t size)
{
	memcpy(out, ptr + std::exchange(offset, offset + size), size);
}

void read_ee_mem_at(auto ptr, size_t at, void* out, size_t size)
{
	memcpy(out, ptr + at, size);
}

std::string read_ee_str(auto ptr, size_t& offset)
{
	const auto len = read_ee<int>(ptr, offset);

	std::string str;

	str.resize(len);

	read_ee_mem(ptr, offset, str.data(), len);

	return str;
}

void FactorySystem::init()
{
#if FAST_LOAD
	set_max_character_spawns(8);
	set_max_vehicle_spawns(8);
#else
	set_max_character_spawns(0);
#endif
}

void FactorySystem::destroy()
{
	// clear our containers

	check(character_handles.empty(), "All character handles must be empty");

	simple_rigid_objects.clear();
	damageables.clear();
	vehicles.clear();
	agent_spawns.clear();
	vehicle_spawns.clear();
	mounted_guns.clear();
	ladders.clear();
	item_pickups.clear();
	animated_rigid_objects.clear();
	ui_map_icons.clear();
	ui_map_icon_types.clear();
	objectives.clear();
	traffic_lights.clear();
	sounds.clear();
}

void FactorySystem::set_max_character_spawns(int v)
{
	jc::write<int16_t>(v, this, jc::spawn_system::MAX_CHARACTER_SPAWNS);
}

void FactorySystem::set_max_vehicle_spawns(int v)
{
	jc::write<int16_t>(v, this, jc::spawn_system::MAX_VEHICLE_SPAWNS);
}

void FactorySystem::destroy_character_handle(CharacterHandle* v)
{
	check(character_handles.contains(v), "Character handle must exists");

	character_handles.erase(v);

	v->destroy();
}

void FactorySystem::destroy_damageable_object(DamageableObject* v)
{
	damageables.erase(v);
}

void FactorySystem::destroy_vehicle(Vehicle* v)
{
	vehicles.erase(v);
}

void FactorySystem::destroy_agent_spawn_point(AgentSpawnPoint* v)
{
	agent_spawns.erase(v);
}

void FactorySystem::destroy_vehicle_spawn_point(VehicleSpawnPoint* v)
{
	vehicle_spawns.erase(v);
}

void FactorySystem::destroy_item_pickup(ItemPickup* v)
{
	item_pickups.erase(v);
}

void FactorySystem::destroy_map_icon(UIMapIcon* v)
{
	ui_map_icons.erase(v);
}

void FactorySystem::destroy_objective(Objective* v)
{
	objectives.erase(v);
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
		character_handles.insert(handle);

		return handle;
	}

	return nullptr;
}

SimpleRigidObject* FactorySystem::spawn_simple_rigid_object(const vec3& position, const std::string& lod_name, const std::string& pfx_name)
{
	Transform transform(position);

	if (auto obj = SimpleRigidObject::ALLOC()->create(&transform, lod_name, pfx_name))
	{
		g_game_control->add_object_to_world(obj);

		return add_to_ref_map(simple_rigid_objects, obj);
	}

	return nullptr;
}

shared_ptr<DamageableObject> FactorySystem::spawn_damageable_object(const vec3& position, const std::string& lod_name, const std::string& pfx_name)
{
	Transform transform(position);

	return DamageableObject::CREATE(&transform, lod_name, pfx_name);
}

shared_ptr<Vehicle> FactorySystem::spawn_vehicle(const std::string& ee_name, const Transform& transform)
{
	shared_ptr<Vehicle> vehicle;

	// check if the EE is from a custom vehicle

	if (ee_name.find("resources") != std::string::npos)
	{
		log(GREEN, "{}", g_custom_vehicle_ees.size());

		if (g_custom_vehicle_ees.contains(ee_name))
		{
			const auto ee_data = util::fs::read_bin_file(ee_name);

			auto curr_ptr = ee_data.data();

			size_t curr_offset = 0ull;

			read_ee<uint32_t>(curr_ptr, curr_offset);
			read_ee<uint32_t>(curr_ptr, curr_offset);
			read_ee<uint32_t>(curr_ptr, curr_offset);

			const auto meta_size = read_ee<uint32_t>(curr_ptr, curr_offset) + 0xC;

			std::vector<AssetInfo> ee_files;

			while (curr_offset < meta_size - 0xC)
			{
				const auto filename = read_ee_str(curr_ptr, curr_offset);
				const auto data_offset = read_ee<uint32_t>(curr_ptr, curr_offset);
				const auto data_size = read_ee<uint32_t>(curr_ptr, curr_offset);

				ee_files.emplace_back(filename, -1, data_offset, data_size);
			}

			// init EE

			const auto ee_resource = ExportedEntityResource::CREATE();
			const auto ee = ee_resource->get_exported_entity();

			// load EE assets

			for (const auto& ee_file : ee_files)
			{
				const auto file_type = g_archives->get_asset_type(ee_file.name);

#if _DEBUG
				log(GREEN, "loading ee file: {} (type: {})", ee_file.name, file_type);
#endif

				std::vector<uint8_t> file_data(ee_file.size);

				read_ee_mem_at(curr_ptr, ee_file.offset, file_data.data(), ee_file.size);


				bool loaded = false;

				while (!loaded)
				{
					jc::stl::string file_name = ee_file.name;

					loaded = jc::this_call<bool>(0x7618A0, ee_resource, &file_name, file_type, file_data.data(), file_data.size());

#if _DEBUG
					log(GREEN, "status ee file: {} (type: {}, loaded: {})", ee_file.name, file_type, loaded);
#endif
				}

				if (ee_file.name.find(".epe") != std::string::npos)
				{
#if _DEBUG
					log(GREEN, "ee: {}", (void*)ee);
					log(GREEN, "name: {} {} {}", ee_file.name, ee_file.offset, ee_file.size);
#endif

					// spawn vehicle

					const auto vehicle_type = VehicleType::CREATE();

					std::string class_name;

					object_base_map* _map = nullptr;

					ee->take_class_property(&class_name, _map);

					vehicle_type->load(class_name, "exported", _map);

					vehicle = vehicle_type->create_vehicle(transform);
					vehicle->enable(true);
					vehicle->set_color(0xFFFFFFFF);

					g_game_control->add_object_to_world(vehicle);

					break;
				}
			}

			ee_resource->free();

			return vehicle;
		}

		return {};
	}

	// make sure we load the ee right now, we don't want to wait
	// for the next frame, we need the vehicle now

	g_rsrc_streamer->request_vehicle_ee(ee_name, [&](ExportedEntityResource* eer, const std::string& name)
	{
		const auto ee = eer->get_exported_entity();

		std::string class_name;

		object_base_map* _map = nullptr;

		if (ee->take_class_property(&class_name, _map))
		{
#if _DEBUG
			_map->walk();
#endif

			const auto vehicle_type = VehicleType::CREATE();

			vehicle_type->load(class_name, name, _map);

			vehicle = vehicle_type->create_vehicle(transform);
			vehicle->enable(true);
			vehicle->set_color(0xFFFFFFFF);

			g_game_control->add_object_to_world(vehicle);

			//add_to_ref_map(vehicles, vehicle);
		}
	}, true);

	return vehicle;
}

AgentSpawnPoint* FactorySystem::create_agent_spawn_point(const vec3& position)
{
	if (auto obj = g_game_control->create_object<AgentSpawnPoint>())
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
		map.insert<object_base_map::Float>(ObjectBase::Hash_SpawnDistance, 500.f);
		map.insert<object_base_map::String>(0xdfe49873, R"(civilian\c_hooker.xml)");
		map.insert<object_base_map::String>(0xa4129310, R"(default_report_msg)");
		map.insert<object_base_map::String>(0x8eb5aff2, R"(exported\agenttypes\civilians\civ_bikinigirl2.ee)");
		map.insert<object_base_map::String>(0x51c3da11, R"(liberation_stop)");
		map.insert<object_base_map::String>(0x78ae93e7, R"(liberation_start)");
		map.insert<object_base_map::String>(0x7c5e1eae, R"(liberation_start)");
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(Beach Girl)");

		obj->init_from_map(&map);
		obj->set_position(position);

		return add_to_ref_map(agent_spawns, obj);
	}

	return nullptr;
}

VehicleSpawnPoint* FactorySystem::create_vehicle_spawn_point(const vec3& position, int32_t id, int32_t faction)
{
	if (auto obj = g_game_control->create_object<VehicleSpawnPoint>())
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
		map.insert<object_base_map::String>(0x8eb5aff2, jc::vars::exported_entities_vehicles[id]);
		// map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, {});

		obj->init_from_map(&map);
		obj->set_position(position);
		obj->set_faction(faction);

		return add_to_ref_map(vehicle_spawns, obj);
	}

	return nullptr;
}

MountedGun* FactorySystem::spawn_mounted_gun(const vec3& position)
{
	if (auto obj = g_game_control->create_object<MountedGun>())
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

		obj->init_from_map(&map);

		return add_to_ref_map(mounted_guns, obj);
	}

	return nullptr;
}

Ladder* FactorySystem::spawn_ladder(const vec3& position, const std::string& model, float length)
{
	if (auto obj = g_game_control->create_object<Ladder>())
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

		obj->init_from_map(&map);

		return add_to_ref_map(ladders, obj);
	}

	return nullptr;
}

ItemPickup* FactorySystem::spawn_general_item_pickup(const vec3& position, uint32_t type, const std::string& model, const std::string& description)
{
	if (auto obj = g_game_control->create_object<ItemPickup>())
	{
		if (!obj->setup(position, type, Weapon_None, model, description))
			return nullptr;

		return add_to_ref_map(item_pickups, obj);
	}

	return nullptr;
}

ItemPickup* FactorySystem::spawn_weapon_item_pickup(const vec3& position, uint32_t weapon_id, const std::string& description)
{
	if (auto obj = g_game_control->create_object<ItemPickup>())
	{
		if (!obj->setup(position, ItemType_Weapon, weapon_id, {}, description))
			return nullptr;

		return add_to_ref_map(item_pickups, obj);
	}

	return nullptr;
}

AnimatedRigidObject* FactorySystem::spawn_animated_rigid_object(const vec3& position, const std::string& model, const std::string& pfx_name)
{
	if (auto obj = g_game_control->create_object<AnimatedRigidObject>())
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

		obj->init_from_map(&map);

		return add_to_ref_map(animated_rigid_objects, obj);
	}

	return nullptr;
}

shared_ptr<UIMapIcon> FactorySystem::create_map_icon(const std::string& name, const vec3& position)
{
	if (auto object = g_game_control->create_object<UIMapIcon>(false))
	{
		if (!object->setup(name, position))
			return {};

		return object;
	}

	return {};
}

UIMapIconType* FactorySystem::create_map_icon_type(const std::string& name, const std::string& texture, const vec2& scale)
{
	if (auto obj = g_game_control->create_object<UIMapIconType>(false))
	{
		if (!obj->setup(name, texture, scale))
			return nullptr;

		return add_to_ref_map(ui_map_icon_types, obj);
	}

	return nullptr;
}

Objective* FactorySystem::create_objective(const vec3& position, const u8vec4& color)
{
	if (auto obj = g_game_control->create_object<Objective>(false))
	{
		if (!obj->setup(position, color))
			return nullptr;

		return add_to_ref_map(objectives, obj);
	}

	return nullptr;
}

TrafficLight* FactorySystem::create_traffic_light(const vec3& position)
{
	if (auto obj = g_game_control->create_object<TrafficLight>(false))
	{
		if (!obj->setup(position))
			return nullptr;

		return add_to_ref_map(traffic_lights, obj);
	}

	return nullptr;
}

SoundGameObject* FactorySystem::create_sound(const vec3& position, const std::string& bank_name, uint32_t sound_id)
{
	if (auto obj = g_game_control->create_object<SoundGameObject>())
	{
		if (!obj->setup(position, bank_name, sound_id))
			return nullptr;

		return add_to_ref_map(sounds, obj);
	}

	return nullptr;
}

shared_ptr<PlayerGrenade> FactorySystem::spawn_grenade(const vec3& position)
{
	Transform transform(position);

	return PlayerGrenade::CREATE(&transform);
}