﻿#ifdef JC_DBG
#include <defs/standard.h>

#include "test_units.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/transform/transform.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/character/character.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/mounted_gun/mounted_gun.h>
#include <game/object/item/item_pickup.h>
#include <game/object/weapon/weapon.h>
#include <game/object/game_resource/ee_resource.h>
#include <game/object/exported_entity/exported_entity.h>
#include <game/object/agent_type/vehicle_type.h>
#include <game/object/agent_type/npc_variant.h>
#include <game/object/rigid_object/animated_rigid_object.h>
#include <game/object/rigid_object/traffic_light.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/sound/sound_game_obj.h>
#include <game/object/mission/objective.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/vehicle/comps/vehicle_seat.h>
#include <game/object/interactable/interactable.h>
#include <game/object/camera/camera.h>
#include <game/object/ui/map_icon.h>
#include <game/object/game_player/game_player.h>
#include <game/object/physics/pfx_collision.h>
#include <game/object/physics/pfx_instance.h>
#include <game/object/camera/cam_settings.h>
#include <game/object/vehicle/car.h>

#include <game/sys/all.h>

#include <serializer/serializer.h>

#include <mp/net.h>

#include <luas.h>

// 40E940 = FnThatReadsAssetFromDisk

DEFINE_HOOK_THISCALL(resource_request, 0x7618A0, bool, ptr a1, jc::stl::string* name, int type, ptr data, ptr size)
{
	/*if (strstr(name->c_str(), "bullet_") || strstr((const char*)data, "360_fire_1"))
	{
		log(PURPLE, "{:x} {:x} {:x} {} {}", a1, data, size, type, name->c_str());
	}*/
	//else log(RED, "{:x} {:x} {:x} {} {}", a1, data, size, type, name->c_str());

	//while (!GetAsyncKeyState(VK_F3));

	const auto result = resource_request_hook(a1, name, type, data, size);;

	//if (type == 12)
	{
		//log(PURPLE, "[{:x}] {:x} {:x} {:x} {} {} ({})", RET_ADDRESS, a1, data, size, type, name->c_str(), result);
	}

	return result;
}

// 8367D0

std::set<std::string> g_collector;

DEFINE_HOOK_THISCALL(_load_model, 0x5C1BD0, bool, int _this, jc::stl::string* name, ptr data, int size)
{
	/*if (strstr(name->c_str(), ".loc"))
	{
		//log(RED, "[{:x}] {} {} {:x} {}", RET_ADDRESS, name->c_str(), type, data, size);

		if (name)
		{
			std::string fixed_name = name->c_str();

			if (const auto it = fixed_name.find_last_of('\\'); it != -1)
				fixed_name = fixed_name.substr(it + 1);

			log(PURPLE, "[{}] {}", g_collector.size(), fixed_name);

			g_collector.insert(fixed_name);
		}

		return _load_model_hook(_this, name, type, data, size);
	}*/

	log(PURPLE, "[{:x}] {:x} {} {}", RET_ADDRESS, _this, data, size);

	return _load_model_hook(_this, name, data, size);
}

DEFINE_HOOK_THISCALL(_test2, 0x55E9D0, ptr, ptr _this, int a1, const char* name)
{
	auto res = _test2_hook(_this, a1, name);

	/*if (name)
	{
		std::string fixed_name = name;

		if (const auto it = fixed_name.find_last_of('\\'); it != -1)
			fixed_name = fixed_name.substr(it + 1);

		if (const auto it = fixed_name.find(".anim"); it != -1)
			fixed_name = fixed_name.substr(0, it);

		log(PURPLE, "[{}] {}", g_collector.size(), fixed_name);

		g_collector.insert(fixed_name);
	}

	if (GetAsyncKeyState(VK_NUMPAD5))
	{
		std::stringstream ss;

		ss << "inline std::unordered_map<int32_t, std::string> animations_list = " << std::endl;
		ss << "{" << std::endl;

		for (int i = 0; const auto& anim : g_collector)
		{
			ss << "\t{ " << i++ << ", ";
			ss << '"';
			ss << anim;
			ss << '"';
			ss << "}," << std::endl;
		}

		ss << "};" << std::endl;

		util::fs::create_text_file("collector.txt", ss.str());
	}*/

	/*if (res && _this != ptr(g_world->get_localplayer_character()))
		log(RED, "2 {:x} {:x}", RET_ADDRESS, _this);*/

	return res;
}

DEFINE_HOOK_THISCALL_S(_test3, 0x596420, bool, int _this)
{
	auto res = _test3_hook(_this);

	/*if (res && _this != ptr(g_world->get_localplayer_character()))
		log(RED, "3 {:x} {:x}", RET_ADDRESS, _this);*/

	return res;
}

std::set<ptr> ay;

DEFINE_HOOK_THISCALL(_test4, 0x5747D0, void, ParticleSystem* ps, jc::stl::string* a2, const char* a3,
	int			 a4,
	int			 a5,
	int			 a6,
	unsigned int a7,
	unsigned int a8)
{
	log(PURPLE, "{}", a3);

	_test4_hook(ps, a2, a3, a4, a5, a6, a7, a8);
}

void jc::test_units::init()
{
	//_test4_hook.hook(true);
	//_test1_hook.hook();
	//_test2_hook.hook();
	//_test3_hook.hook();
	//_test_hook.hook();

	//resource_request_hook.hook();
	//_load_model_hook.hook();
}

void jc::test_units::destroy()
{
	//_test4_hook.hook(false);
	//_test1_hook.unhook();
	//_test2_hook.unhook();
	//_test3_hook.unhook();
	//_test4_hook.unhook();
	//_test_hook.unhook();

	//resource_request_hook.unhook();
	//_load_model_hook.unhook();
}

shared_ptr<Vehicle> test_vehicle;

void jc::test_units::test_0()
{
	auto localplayer = g_world->get_local();

	auto local_char = g_world->get_local_character();

	if (!local_char)
		return;

	auto local_pos = local_char->get_position();

	Transform local_t(local_pos);

	static std::vector<shared_ptr<Vehicle>> vehs;
	static std::vector<shared_ptr<Weapon>> temp_weapons;
	static std::vector<shared_ptr<Vehicle>> temp_vehicles;

	/*if (g_key->is_key_pressed(KEY_NUM_5))
	{
		std::stringstream ss;

		ss << "inline std::vector<std::string> pfxs_list = " << std::endl;
		ss << "{" << std::endl;

		for (const auto& item : g_collector)
		{
			ss << "\t";
			ss << '"';
			ss << item;
			ss << '"';
			ss << "," << std::endl;
		}

		ss << "};" << std::endl;

		util::fs::create_text_file("collector.txt", ss.str());
	}*/

	if (g_global_ptr)
	{
		//jc::write(0xFF0000FF, g_global_ptr, 0xDC);
		//jc::this_call(0x854A00, g_global_ptr, 2, true);
	}

	static int aaa = 0;

	/*uint8_t arr[] = { 0xB8, 0x00, 0x00, 0x00, 0x01 };

	jc::write(0x76ui8, 0x4C48A3);
	jc::write(0x76ui8, 0x4C48DB);*/

	if (g_key->is_key_pressed(VK_NUMPAD4)) 
	{
		/*const std::string anim = "dance_hooker_NPC_2.anim";

		g_anim_system->load_anim_async(anim);

		local_char->set_animation("dance_hooker_NPC_2.anim", 0.2f, true, false);*/

		const std::string een = "lave_003_Normal_Car_2.ee";

		g_rsrc_streamer->request_vehicle_ee(een, [&](ExportedEntityResource*, const std::string&)
		{
			object_base_map map {};

			{
				const auto m0 = mat4 { 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f };
				const auto m1 = mat4 { 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f };
				const auto m3 = mat4 { -1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, -1.00f, 0.00f, -0.25f, 1.90f, -0.20f, 1.00f };
				const auto m4 = mat4 { -1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, -1.00f, 0.00f, -0.50f, 0.56f, -0.60f, 1.00f };
				const auto m5 = mat4 { 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f };
				const auto m6 = mat4 { 0.60f, 0.00f, 0.00f, 0.00f, 0.00f, 0.60f, 0.00f, 0.00f, 0.00f, 0.00f, 0.70f, 0.00f, 1.00f, 0.60f, 0.00f, 1.00f };
				const auto m2 = mat4 { -1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, -1.00f, 0.00f, 0.00f, 1.85f, 2.30f, 1.00f }; // special seat
				const auto m7 = mat4 { 2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f }; // special seat
				const auto v3_0 = vec3 { 0.30f, 0.28f, 1.30f }; // special seat
				const auto v3_5 = vec3 { 0.00f, 0.00f, 0.00f }; // special seat
				const auto v3_8 = vec3 { -0.12f, 0.06f, 0.56f }; // special seat
				const auto v3_9 = vec3 { 0.12f, 0.06f, 0.56f }; // special seat
				const auto v3_14 = vec3 { 0.00f, -1.33f, 0.50f }; // special seat
				const auto v3_1 = vec3 { 0.00f, 2.00f, -0.20f };
				const auto v3_2 = vec3 { 0.00f, 0.00f, 0.00f };
				const auto v3_3 = vec3 { 0.00f, 1.00f, -2.00f };
				const auto v3_4 = vec3 { 0.00f, 1.00f, -2.00f };
				const auto v3_6 = vec3 { -1.00f, 4.00f, 0.00f };
				const auto v3_7 = vec3 { 0.00f, 0.00f, 0.00f };
				const auto v3_10 = vec3 { -1.00f, 3.00f, 0.00f };
				const auto v3_11 = vec3 { 0.00f, 0.00f, 0.00f };
				const auto v3_12 = vec3 { 0.00f, 0.00f, 0.00f };
				const auto v3_13 = vec3 { -1.00f, 5.00f, 0.00f };
				const auto v3_15 = vec3 { 0.00f, 1.05f, 1.97f };
				map.insert<object_base_map::Int>(0x2f61d80e, 1); // int - use mounted gun
				map.insert<object_base_map::Int>(0x10cea63e, 1); // int
				map.insert<object_base_map::Int>(0x1bdbd87f, 0); // int
				map.insert<object_base_map::Int>(0x29abd0e7, 0); // int
				map.insert<object_base_map::Int>(0x2d27d71, 0); // int
				map.insert<object_base_map::Int>(0x2e8bae0c, 1); // int
				map.insert<object_base_map::Int>(0x3366da33, 0); // int
				map.insert<object_base_map::Int>(0x37595a1a, 10); // int
				map.insert<object_base_map::Int>(0x3d9c4b55, 0); // int
				map.insert<object_base_map::Int>(0x3f23e8c2, 0); // int
				map.insert<object_base_map::Int>(0x4b049cc6, 1); // int
				map.insert<object_base_map::Int>(0x57d204a4, 1); // int
				map.insert<object_base_map::Int>(0x5aa89eea, 0); // int
				map.insert<object_base_map::Int>(0x5eee6eac, 0); // int
				map.insert<object_base_map::Int>(0x67372b66, 1); // int
				map.insert<object_base_map::Int>(0x6a5fa77b, 0); // int
				map.insert<object_base_map::Int>(0x6d629c64, 1); // int
				map.insert<object_base_map::Int>(0x7e0179b, -1); // int
				map.insert<object_base_map::Int>(0x89c55210, 0); // int
				map.insert<object_base_map::Int>(0x9554e532, 0); // int
				map.insert<object_base_map::Int>(0xa14f22a2, 0); // int
				map.insert<object_base_map::Int>(0xaa30db97, 0); // int
				map.insert<object_base_map::Int>(0xab890cf0, 1); // int
				map.insert<object_base_map::Int>(0xb03f76dd, 1); // int
				map.insert<object_base_map::Int>(0xb4b8a5e4, 0); // int
				map.insert<object_base_map::Int>(0xbd56c9e8, 1); // int
				map.insert<object_base_map::Int>(0xc34b70fd, 0); // int
				map.insert<object_base_map::Int>(0xcf46bde6, 1); // int
				map.insert<object_base_map::Int>(0xd078dd0f, 0); // int
				map.insert<object_base_map::Int>(0xd8d7e8f2, 1); // int
				map.insert<object_base_map::Int>(0xeb0939c3, 1); // int
				map.insert<object_base_map::Int>(0xffa9a8a2, 1); // int
				map.insert<object_base_map::Float>(0x1106b477, 1.00f); // float
				map.insert<object_base_map::Float>(0x11713c81, 0.00f); // float
				map.insert<object_base_map::Float>(0x1a379f4c, 8.00f); // float
				map.insert<object_base_map::Float>(0x1ad78bda, 1.00f); // float
				map.insert<object_base_map::Float>(0x1c7aa160, 0.70f); // float
				map.insert<object_base_map::Float>(0x1f61e9a, -0.10f); // float
				map.insert<object_base_map::Float>(0x27cc929a, -8.00f); // float
				map.insert<object_base_map::Float>(0x29ae9b9d, 0.75f); // float
				map.insert<object_base_map::Float>(0x3017bac1, 3.10f); // float
				map.insert<object_base_map::Float>(0x34ac2438, 1.00f); // float
				map.insert<object_base_map::Float>(0x38235d93, 0.00f); // float
				map.insert<object_base_map::Float>(0x3b6ca735, 4.00f); // float
				map.insert<object_base_map::Float>(0x42e6f844, 4.00f); // float
				map.insert<object_base_map::Float>(0x55506c93, 30.00f); // float
				map.insert<object_base_map::Float>(0x555b0b3a, 0.90f); // float
				map.insert<object_base_map::Float>(0x67dfb4e9, 3.00f); // float
				map.insert<object_base_map::Float>(0x68f17609, 0.20f); // float
				map.insert<object_base_map::Float>(0x71b9da94, -0.75f); // float
				map.insert<object_base_map::Float>(0x79fc37d6, -0.90f); // float
				map.insert<object_base_map::Float>(0x8515daf3, 100.00f); // float
				map.insert<object_base_map::Float>(0x9b92cd11, -0.75f); // float
				map.insert<object_base_map::Float>(0x9c6cd67c, 100.00f); // float
				map.insert<object_base_map::Float>(0x9fcd3bc1, 0.75f); // float
				map.insert<object_base_map::Float>(0xa2edb6fe, 0.00f); // float
				map.insert<object_base_map::Float>(0xa5a549bd, 0.10f); // float
				map.insert<object_base_map::Float>(0xa94e99e4, 65.00f); // float
				map.insert<object_base_map::Float>(0xcf244184, 0.00f); // float
				map.insert<object_base_map::Float>(0xe652aac5, 0.30f); // float
				map.insert<object_base_map::Float>(0xed737006, 0.30f); // float
				map.insert<object_base_map::Float>(0xee13ff61, 0.20f); // float
				map.insert<object_base_map::Float>(0xf4787d50, 1.00f); // float
				map.insert<object_base_map::Float>(0xfa10a8d, 100.00f); // float
				map.insert<object_base_map::Float>(0xfe31029d, 0.20f); // float
				map.insert<object_base_map::Float>(0xfff34f1e, 0.40f); // float
				map.insert<object_base_map::String>(0x13fca49, R"()"); // string
				map.insert<object_base_map::String>(0x1616c9e6, R"()"); // string
				map.insert<object_base_map::String>(0x173af671, R"()"); // string
				{
					map.insert<object_base_map::String>(0x274e57fb, R"(Mounted M60)"); // string
					map.insert<object_base_map::String>(0xa4ab5487, R"(weapons\weap_011_groundmount.lod)"); // string
				}
				map.insert<object_base_map::String>(0x2a748f45, R"()"); // string
				map.insert<object_base_map::String>(0x2bd18132, R"()"); // string
				map.insert<object_base_map::String>(0x2f4a44f5, R"()"); // string
				map.insert<object_base_map::String>(0x3921ad5f, R"()"); // string
				map.insert<object_base_map::String>(0x3e3cf364, R"()"); // string
				//map.insert<object_base_map::String>(0x3e730608, R"(MODIFIER_VEHICLE_CAR)"); // string
				map.insert<object_base_map::String>(0x475717a8, R"()"); // string
				map.insert<object_base_map::String>(0x4a1c81fc, R"()"); // string - Vehicles\Land_vehicles\LAVE_003\LAVE_003_tire_d.lod
				map.insert<object_base_map::String>(0x5408f582, R"()"); // string
				map.insert<object_base_map::String>(0x62b03494, R"(lave_003.pfx)"); // string
				//map.insert<object_base_map::String>(0x62b03494, R"(lave_004.pfx)"); // string
				map.insert<object_base_map::String>(0x662ada86, R"()"); // string
				map.insert<object_base_map::String>(0x7bdf0a4b, R"()"); // string
				map.insert<object_base_map::String>(0x7fa14b9d, R"()"); // string
				map.insert<object_base_map::String>(0x80a47a4, R"()"); // string
				map.insert<object_base_map::String>(0x833885e9, R"()"); // string
				map.insert<object_base_map::String>(0x848f22dc, R"()"); // string
				map.insert<object_base_map::String>(0x85afa691, R"()"); // string
				map.insert<object_base_map::String>(0x8c86f0e, R"()"); // string
				map.insert<object_base_map::String>(0xa4ab5487, R"()"); // string
				map.insert<object_base_map::String>(0xb0ecdc44, R"(Carpaint_Standard.bmp)"); // string
				map.insert<object_base_map::String>(0xc16ed911, R"(Amogus Car)"); // string
				map.insert<object_base_map::String>(0xc3a589bc, R"()"); // string
				map.insert<object_base_map::String>(0xc49eb28a, R"()"); // string
				map.insert<object_base_map::String>(0xdaf82b95, R"()"); // string
				map.insert<object_base_map::String>(0xe2cfbec7, R"(lave_004_tire_lod1.rbm)"); // string
				map.insert<object_base_map::String>(0xe48362f, R"()"); // string
				map.insert<object_base_map::String>(0xe6420790, R"()"); // string
				//map.insert<object_base_map::String>(0xe82d4eb5, R"(models\vehicles\land_vehicles\lave_003\lave_003.pfx)"); // string
				map.insert<object_base_map::String>(0xe86cc05c, R"()"); // string
				//map.insert<object_base_map::String>(0xfbb640e5, R"(Models\Vehicles\Land_vehicles\LAVE_003\LAVE_003.dmf)"); // string
				map.insert<object_base_map::String>(0xfbb640e5, R"()"); // string
				map.insert<object_base_map::String>(ObjectBase::Hash_Class, R"(CCar)"); // string
				map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(Amogus Car)"); // string
				//map.insert<object_base_map::String>(ObjectBase::Hash_Model, R"()"); // string
				map.insert<object_base_map::String>(ObjectBase::Hash_Model, R"(badboll_lod1pc.rbm)"); // string
				map.insert<object_base_map::Vec3>(0x20ad9efa, &v3_0); // vec3
				map.insert<object_base_map::Vec3>(0x2bcceaa0, &v3_2); // vec3
				map.insert<object_base_map::Vec3>(0x2da89476, &v3_1); // vec3
				map.insert<object_base_map::Vec3>(0x2de4b77e, &v3_3); // vec3
				map.insert<object_base_map::Vec3>(0x329b41fd, &v3_4); // vec3
				map.insert<object_base_map::Vec3>(0x489f0f01, &v3_5); // vec3
				map.insert<object_base_map::Vec3>(0x6a894c31, &v3_6); // vec3
				map.insert<object_base_map::Vec3>(0x6d15b7fa, &v3_7); // vec3
				map.insert<object_base_map::Vec3>(0x7ebe23fd, &v3_8); // vec3
				map.insert<object_base_map::Vec3>(0x82ba0ccf, &v3_9); // vec3
				map.insert<object_base_map::Vec3>(0x93ae0049, &v3_10); // vec3
				map.insert<object_base_map::Vec3>(0x9f8bd56f, &v3_11); // vec3
				map.insert<object_base_map::Vec3>(0xc41e4e80, &v3_12); // vec3
				map.insert<object_base_map::Vec3>(0xd02c4ca9, &v3_13); // vec3
				map.insert<object_base_map::Vec3>(0xf191bbc0, &v3_14); // vec3
				map.insert<object_base_map::Vec3>(0xf58e9028, &v3_15); // vec3
				map.insert<object_base_map::Mat4>(0x1c65cac3, &m1); // mat4
				map.insert<object_base_map::Mat4>(0x316ec15e, &m2); // mat4
				map.insert<object_base_map::Mat4>(0x3547f82d, &m3); // mat4
				map.insert<object_base_map::Mat4>(0x5796b6e7, &m4); // mat4
				map.insert<object_base_map::Mat4>(0x61786c8d, &m5); // mat4
				map.insert<object_base_map::Mat4>(0x6d362dd4, &m6); // mat4
				map.insert<object_base_map::Mat4>(0xb5be7095, &m7); // mat4
				map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, &m0); // mat4
			}

			TransformTR transform(local_t.get_position() + vec3(4.f, 1.f, 0.f));

			auto veh_trans = Transform(transform.t, transform.r);

			/*if (!g_model_system->has_resource("lave_004_tire_lod1.lod"))
			{
				g_model_system->load_rbm("lave_004_tire_lod1.rbm", g_archives->get_asset_data("lave_004_tire_lod1.rbm"));
			}

			if (!g_physics->has_resource("lave_004.pfx"))
			{
				auto rbm = util::fs::read_bin_file("lave_004.pfx");

				g_physics->load_pfx("lave_004.pfx", rbm, &veh_trans);
			}*/

			const auto vehicle_type = VehicleType::CREATE();

			vehicle_type->load("CCar", "exported", &map);

			test_vehicle = vehicle_type->create_vehicle(veh_trans);
			test_vehicle->enable(true);
			test_vehicle->set_color(0xFFFFFFFF);

			g_game_control->add_object_to_world(test_vehicle);
		}, true);

		/*if (auto belt = local_char->get_weapon_belt())
			belt->clear();*/

		/*if (const auto new_player = g_net->get_random_player())
		{
			shared_ptr<Character>* ayptr = new_player != g_net->get_localplayer() ?
				(shared_ptr<Character>*)(ptr(new_player->get_character_handle()) + 0x11C) :
				(shared_ptr<Character>*)(ptr(g_world->get_local()) + 0x1C);

			g_cam_control->get_settings()->set_character(*ayptr);
		}*/

		//log(RED, "current {}", --aaa);
	}

	//if (auto entry = g_archives->get_asset_entry(R"(Models\Characters\Animations\NPCMoves\hooker\dance_hooker_NPC_1.anim)"))
	//	log(YELLOW, "{:x} {:x} {:x}", entry->hash, entry->offset, entry->size);

	static std::vector<shared_ptr<Weapon>> trash;

	if (g_key->is_key_pressed(VK_NUMPAD5))
	{
		const std::string een = "arve_022_old_school_fighter.ee";

		g_rsrc_streamer->request_vehicle_ee(een, [&](ExportedEntityResource* eer, const std::string&)
		{
			object_base_map map {};

			{
				const auto m0 = mat4 { 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f };
				const auto m1 = mat4 { 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f };
				const auto m3 = mat4 { -1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, -1.00f, 0.00f, -0.76f, 2.70f, 5.00f, 1.00f };
				const auto m4 = mat4 { -1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, -1.00f, 0.00f, 0.00f, 1.10f, -0.30f, 1.00f };
				const auto m5 = mat4 { -1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, -1.00f, 0.00f, 0.00f, 1.50f, 0.00f, 1.00f };
				const auto m6 = mat4 { -3.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.70f, 0.00f, 0.00f, 0.00f, 0.00f, -4.00f, 0.00f, 0.00f, 0.40f, -0.30f, 1.00f };
				const auto m2 = mat4 { -1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, -1.00f, 0.00f, 0.00f, 2.85f, 2.30f, 1.00f }; // special seat - relative position
				const auto m7 = mat4 { 2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f }; // special seat
				const auto v3_0 = vec3 { 0.30f, 0.52f, 7.5f }; // special seat - relative camera position
				const auto v3_6 = vec3 { -1.00f, 4.00f, 0.00f }; // special seat
				const auto v3_9 = vec3 { 0.18f, 0.06f, 0.56f }; // special seat
				const auto v3_10 = vec3 { 0.10f, 1.28f, 4.09f }; // special seat
				const auto v3_15 = vec3 { 0.f, -1.05f, 0.7f }; // special seat - relative character position
				const auto v3_1 = vec3 { -5.22f, 1.08f, -0.41f };
				const auto v3_2 = vec3 { -0.10f, 2.05f, -0.10f };
				const auto v3_3 = vec3 { 0.00f, 0.00f, 0.00f };
				const auto v3_4 = vec3 { 0.00f, 1.30f, -2.90f };
				const auto v3_5 = vec3 { 0.00f, 1.30f, -2.90f };
				const auto v3_7 = vec3 { 0.00f, 0.00f, 0.00f };
				const auto v3_8 = vec3 { -1.77f, 1.53f, 1.13f };
				const auto v3_11 = vec3 { -1.00f, 3.00f, 0.00f };
				const auto v3_12 = vec3 { 0.00f, 0.00f, 0.00f };
				const auto v3_13 = vec3 { 0.00f, 0.00f, 0.00f };
				const auto v3_14 = vec3 { -1.00f, 5.00f, 0.00f };
				const auto v3_16 = vec3 { 0.01f, 1.51f, 4.27f };
				map.insert<object_base_map::Int>(0x2f61d80e, 1); // int - use MG
				map.insert<object_base_map::Int>(0x10cea63e, 1); // int
				map.insert<object_base_map::Int>(0x29abd0e7, 1); // int
				map.insert<object_base_map::Int>(0x2e8bae0c, 1); // int
				map.insert<object_base_map::Int>(0x3366da33, 0); // int
				map.insert<object_base_map::Int>(0x37595a1a, 10); // int
				map.insert<object_base_map::Int>(0x4b049cc6, 1); // int
				map.insert<object_base_map::Int>(0x57d204a4, 1); // int
				map.insert<object_base_map::Int>(0x5aa89eea, 0); // int
				map.insert<object_base_map::Int>(0x5eee6eac, 0); // int
				map.insert<object_base_map::Int>(0x67372b66, 0); // int
				map.insert<object_base_map::Int>(0x6a5fa77b, 0); // int
				map.insert<object_base_map::Int>(0x6d629c64, 1); // int
				map.insert<object_base_map::Int>(0x7e0179b, -1); // int
				map.insert<object_base_map::Int>(0x9554e532, 0); // int
				map.insert<object_base_map::Int>(0xaa30db97, 0); // int
				map.insert<object_base_map::Int>(0xab890cf0, 1); // int
				map.insert<object_base_map::Int>(0xb03f76dd, 1); // int
				map.insert<object_base_map::Int>(0xb4b8a5e4, 1); // int
				map.insert<object_base_map::Int>(0xbd56c9e8, 1); // int
				map.insert<object_base_map::Int>(0xc34b70fd, 0); // int
				map.insert<object_base_map::Int>(0xcf46bde6, 1); // int
				map.insert<object_base_map::Int>(0xd078dd0f, 0); // int
				map.insert<object_base_map::Int>(0xd8d7e8f2, 1); // int
				map.insert<object_base_map::Int>(0xeb0939c3, 1); // int
				map.insert<object_base_map::Int>(0xffa9a8a2, 1); // int
				map.insert<object_base_map::Float>(0x1106b477, 300.00f); // float
				map.insert<object_base_map::Float>(0x1a379f4c, 8.00f); // float
				map.insert<object_base_map::Float>(0x1ad78bda, 0.70f); // float
				map.insert<object_base_map::Float>(0x1f61e9a, -1.00f); // float
				map.insert<object_base_map::Float>(0x29ae9b9d, 0.75f); // float
				map.insert<object_base_map::Float>(0x3017bac1, 5.00f); // float
				map.insert<object_base_map::Float>(0x34ac2438, 300.00f); // float
				map.insert<object_base_map::Float>(0x38235d93, 0.00f); // float
				map.insert<object_base_map::Float>(0x3b6ca735, 0.09f); // float
				map.insert<object_base_map::Float>(0x45edd054, 0.05f); // float
				map.insert<object_base_map::Float>(0x55506c93, 300.00f); // float
				map.insert<object_base_map::Float>(0x555b0b3a, 1.00f); // float
				map.insert<object_base_map::Float>(0x6164be27, 0.50f); // float
				map.insert<object_base_map::Float>(0x67de3b7b, 0.25f); // float
				map.insert<object_base_map::Float>(0x67dfb4e9, 3.20f); // float
				map.insert<object_base_map::Float>(0x6ab06c4c, 0.40f); // float
				map.insert<object_base_map::Float>(0x71b9da94, -0.75f); // float
				map.insert<object_base_map::Float>(0x79fc37d6, -1.00f); // float
				map.insert<object_base_map::Float>(0x8515daf3, 100.00f); // float
				map.insert<object_base_map::Float>(0x9b92cd11, -0.75f); // float
				map.insert<object_base_map::Float>(0x9c6cd67c, 100.00f); // float
				map.insert<object_base_map::Float>(0x9fcd3bc1, 0.75f); // float
				map.insert<object_base_map::Float>(0xa94e99e4, 65.00f); // float
				map.insert<object_base_map::Float>(0xac7f1127, 0.07f); // float
				map.insert<object_base_map::Float>(0xb16fe83c, 0.14f); // float
				map.insert<object_base_map::Float>(0xb189689d, 0.05f); // float
				map.insert<object_base_map::Float>(0xb5411373, 0.70f); // float
				map.insert<object_base_map::Float>(0xcd1d1480, 100.00f); // float
				map.insert<object_base_map::Float>(0xcf244184, 0.00f); // float
				map.insert<object_base_map::Float>(0xdb12e91d, 0.70f); // float
				map.insert<object_base_map::Float>(0xdba4c62f, 0.25f); // float
				map.insert<object_base_map::Float>(0xed737006, 0.30f); // float
				map.insert<object_base_map::Float>(0xf411cb9e, 0.07f); // float
				map.insert<object_base_map::Float>(0xf4787d50, 1.00f); // float
				map.insert<object_base_map::Float>(0xfa10a8d, 100.00f); // float
				map.insert<object_base_map::Float>(0xfe31029d, 1.00f); // float
				map.insert<object_base_map::Float>(0xfe56916c, 0.09f); // float
				map.insert<object_base_map::Float>(0xfff34f1e, 0.40f); // float
				map.insert<object_base_map::String>(0x13fca49, R"()"); // string
				map.insert<object_base_map::String>(0x173af671, R"()"); // string
				map.insert<object_base_map::String>(0x238dda1c, R"()"); // string
				map.insert<object_base_map::String>(0x274e57fb, R"(Mounted M60)"); // string
				map.insert<object_base_map::String>(0x2a748f45, R"()"); // string
				map.insert<object_base_map::String>(0x2bd18132, R"()"); // string
				map.insert<object_base_map::String>(0x3921ad5f, R"()"); // string
				map.insert<object_base_map::String>(0x3e3cf364, R"()"); // string
				map.insert<object_base_map::String>(0x3e730608, R"(MODIFIER_VEHICLE_CAR)"); // string
				map.insert<object_base_map::String>(0x475717a8, R"()"); // string
				map.insert<object_base_map::String>(0x4a1c81fc, R"(Vehicles\Air_vehicles\ARVE_022\ARVE_022_wheel.lod)"); // string
				map.insert<object_base_map::String>(0x5408f582, R"()"); // string
				map.insert<object_base_map::String>(0x62b03494, R"(Models\Vehicles\Air_vehicles\ARVE_022\ARVE_022.pfx)"); // string
				map.insert<object_base_map::String>(0x662ada86, R"()"); // string
				map.insert<object_base_map::String>(0x7bdf0a4b, R"()"); // string
				map.insert<object_base_map::String>(0x7fa14b9d, R"()"); // string
				map.insert<object_base_map::String>(0x80a47a4, R"()"); // string
				map.insert<object_base_map::String>(0x833885e9, R"()"); // string
				map.insert<object_base_map::String>(0x848f22dc, R"()"); // string
				map.insert<object_base_map::String>(0x85afa691, R"()"); // string
				map.insert<object_base_map::String>(0x8c86f0e, R"()"); // string
				map.insert<object_base_map::String>(0x9726f27d, R"(vehicles\air_vehicles\arve_022\arve_022_propeller_alpha.lod)"); // string
				map.insert<object_base_map::String>(0xa4ab5487, R"()"); // string
				map.insert<object_base_map::String>(0xb0ecdc44, R"(black.bmp)"); // string
				map.insert<object_base_map::String>(0xc16ed911, R"(McKenzie-Fergusson F2 Victor)"); // string
				map.insert<object_base_map::String>(0xc3a589bc, R"()"); // string
				map.insert<object_base_map::String>(0xc6660280, R"(Vehicles\Air_vehicles\ARVE_022\ARVE_022_propeller.lod)"); // string
				map.insert<object_base_map::String>(0xcd076edb, R"()"); // string
				map.insert<object_base_map::String>(0xdaf82b95, R"()"); // string
				map.insert<object_base_map::String>(0xe2cfbec7, R"(Vehicles\Air_vehicles\ARVE_022\ARVE_022_wheel.lod)"); // string
				map.insert<object_base_map::String>(0xe48362f, R"()"); // string
				map.insert<object_base_map::String>(0xe6420790, R"()"); // string
				map.insert<object_base_map::String>(0xe82d4eb5, R"(models\vehicles\air_vehicles\arve_014\arve_014.pfx)"); // string
				map.insert<object_base_map::String>(0xe86cc05c, R"()"); // string
				map.insert<object_base_map::String>(0xed54a2bb, R"()"); // string
				map.insert<object_base_map::String>(0xfbb640e5, R"(Models\Vehicles\Air_vehicles\ARVE_022\ARVE_022.dmf)"); // string
				map.insert<object_base_map::String>(ObjectBase::Hash_Class, R"(CAirPlane)"); // string
				map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(CAirPlane1)"); // string
				map.insert<object_base_map::String>(ObjectBase::Hash_Model, R"(Vehicles\Air_vehicles\ARVE_022\ARVE_022_body.lod)"); // string
				map.insert<object_base_map::Vec3>(0x1ab1c1b0, &v3_1); // vec3
				map.insert<object_base_map::Vec3>(0x20ad9efa, &v3_0); // vec3
				map.insert<object_base_map::Vec3>(0x2bcceaa0, &v3_3); // vec3
				map.insert<object_base_map::Vec3>(0x2da89476, &v3_2); // vec3
				map.insert<object_base_map::Vec3>(0x2de4b77e, &v3_4); // vec3
				map.insert<object_base_map::Vec3>(0x329b41fd, &v3_5); // vec3
				map.insert<object_base_map::Vec3>(0x489f0f01, &v3_6); // vec3
				map.insert<object_base_map::Vec3>(0x51319039, &v3_7); // vec3
				map.insert<object_base_map::Vec3>(0x6a894c31, &v3_8); // vec3
				map.insert<object_base_map::Vec3>(0x7ebe23fd, &v3_9); // vec3
				map.insert<object_base_map::Vec3>(0x82ba0ccf, &v3_10); // vec3
				map.insert<object_base_map::Vec3>(0x93ae0049, &v3_11); // vec3
				map.insert<object_base_map::Vec3>(0x992e85c5, &v3_12); // vec3
				map.insert<object_base_map::Vec3>(0xc41e4e80, &v3_13); // vec3
				map.insert<object_base_map::Vec3>(0xd02c4ca9, &v3_14); // vec3
				map.insert<object_base_map::Vec3>(0xf191bbc0, &v3_15); // vec3
				map.insert<object_base_map::Vec3>(0xf58e9028, &v3_16); // vec3
				map.insert<object_base_map::Mat4>(0x1c65cac3, &m1); // mat4
				map.insert<object_base_map::Mat4>(0x316ec15e, &m2); // mat4
				map.insert<object_base_map::Mat4>(0x3547f82d, &m3); // mat4
				map.insert<object_base_map::Mat4>(0x5796b6e7, &m4); // mat4
				map.insert<object_base_map::Mat4>(0x61786c8d, &m5); // mat4
				map.insert<object_base_map::Mat4>(0x6d362dd4, &m6); // mat4
				map.insert<object_base_map::Mat4>(0xb5be7095, &m7); // mat4
				map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, &m0); // mat4
			}

			TransformTR transform(local_t.get_position() + vec3(4.f, 1.f, 0.f));

			auto veh_trans = Transform(transform.t, transform.r);

			/*if (!g_model_system->has_resource("lave_004_tire_lod1.lod"))
			{
				g_model_system->load_rbm("lave_004_tire_lod1.rbm", g_archives->get_asset_data("lave_004_tire_lod1.rbm"));
			}

			if (!g_physics->has_resource("lave_004.pfx"))
			{
				auto rbm = util::fs::read_bin_file("lave_004.pfx");

				g_physics->load_pfx("lave_004.pfx", rbm, &veh_trans);
			}*/

			const auto vehicle_type = VehicleType::CREATE();

			std::string class_name;

			object_base_map* _map = nullptr;

			eer->get_exported_entity()->take_class_property(&class_name, _map);

			vehicle_type->load(class_name, "exported", &map);

			test_vehicle = vehicle_type->create_vehicle(veh_trans);
			test_vehicle->enable(true);
			test_vehicle->set_color(0xFFFFFFFF);

			g_game_control->add_object_to_world(test_vehicle);
		}, true);
	}
	
	if (g_key->is_key_pressed(VK_ADD))
	{
		
	}

	/*for (const auto& w : temp_weapons)
	{
		//w->set_transform(local_t);
		//w->set_last_shot_time(jc::nums::MAXF);
		w->set_last_muzzle_transform(local_t);
		//w->set_muzzle_position(local_pos);
		//w->set_aim_target(vec3(0.f, 0.f, 1.f));
		w->force_fire();
		w->set_enabled(true);
		w->update();
	}*/

	struct TestInfo
	{
		CharacterHandle* handle = nullptr;
		Character* character = nullptr;
		Vehicle* vehicle = nullptr;
	} static info;

	/*if (auto veh_seat = local_char->get_vehicle_seat())
		if (auto veh = veh_seat->get_vehicle())
			info.vehicle = veh;*/

	if (g_key->is_key_pressed(VK_F8))
	{
		if (info.character)
		{
			g_factory->destroy_character_handle(info.handle);

			info.handle = nullptr;
			info.character = nullptr;
		}
	}

	/*if (g_key->is_key_pressed(VK_NUMPAD8))
	{
		auto mem = jc::game::malloc(0x17C);

		std::string model = "m488.rbm"; // "crate_custom_png.rbm"

		object_base_map map{};
		map.insert<object_base_map::Int>(0x2419daa1, 15); // int
		map.insert<object_base_map::Int>(0x2caec4c6, 3); // int
		map.insert<object_base_map::Int>(0x2e3e2094, 1); // int
		map.insert<object_base_map::Int>(0x2ecc28f9, -1); // int
		map.insert<object_base_map::Int>(0x3c6afa74, -1); // int
		map.insert<object_base_map::Int>(0x67f4b92, 1); // int
		map.insert<object_base_map::Int>(0x73c279d9, 7); // int
		map.insert<object_base_map::Int>(0x7932fd00, 3); // int
		map.insert<object_base_map::Int>(0x92d1b4b0, 1); // int
		map.insert<object_base_map::Int>(0x983bdc19, 16); // int
		map.insert<object_base_map::Int>(0xb48c8992, 14); // int
		map.insert<object_base_map::Int>(0xc17ff89f, 100); // int
		map.insert<object_base_map::Int>(0xdc4e8e89, 12); // int
		map.insert<object_base_map::Int>(0xe43f6ff8, 0); // int
		map.insert<object_base_map::Int>(0xea9b2d84, 2); // int
		map.insert<object_base_map::Int>(0xebb4f93f, 1); // int
		map.insert<object_base_map::Int>(0xf776041b, 1); // int
		map.insert<object_base_map::Float>(0x2524eee2, 7.00f); // float
		map.insert<object_base_map::Float>(0x3ae7b611, 500.00f); // float
		map.insert<object_base_map::Float>(0x3c973815, 1.00f); // float
		map.insert<object_base_map::Float>(0x43d3fa78, 1.00f); // float
		map.insert<object_base_map::Float>(0x51905fda, 50.00f); // float
		map.insert<object_base_map::Float>(0x58dd9170, 50.00f); // float - max radius
		map.insert<object_base_map::Float>(0x61d710ac, 100.00f); // float
		map.insert<object_base_map::Float>(0x6e24e123, 1.50f); // float
		map.insert<object_base_map::Float>(0x72fdb359, 500.00f); // float
		map.insert<object_base_map::Float>(0x737cf1df, 1.25f); // float
		map.insert<object_base_map::Float>(0x83fcbe6d, 0.32f); // float
		map.insert<object_base_map::Float>(0x88bbee10, 1.20f); // float
		map.insert<object_base_map::Float>(0x8dccf8a, 200.00f); // float
		map.insert<object_base_map::Float>(0x9db0dcdf, 130.00f); // float
		map.insert<object_base_map::Float>(0xb27fbdf, 9000.00f); // float
		map.insert<object_base_map::Float>(0xc5a583e4, 7.00f); // float
		map.insert<object_base_map::Float>(0xc9dae566, 1.00f); // float
		map.insert<object_base_map::Float>(0xe775a5da, 2.00f); // float
		map.insert<object_base_map::Float>(0xe807fbbe, 0.10f); // float
		map.insert<object_base_map::Float>(0xf400ec2a, 1.00f); // float
		map.insert<object_base_map::Float>(0xfdc2fc1f, 60.00f); // float
		map.insert<object_base_map::Float>(0xfde6d38c, 0.01f); // float
		map.insert<object_base_map::String>(0xa9818615, R"(VFX_I_Muzzle_Rocket_Launcher)"); // string
		map.insert<object_base_map::String>(0xb231ec06, R"(M488)"); // string
		map.insert<object_base_map::String>(0xb3776904, R"(360_exp_4_1)"); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(Rocket Launcher)"); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_Model, model); // string

		g_texture_system->load_texture("dummy_black.dds");
		g_model_system->load_rbm(model);

		jc::this_call(0x713D00, mem);
		jc::this_call(0x7142B0, mem, &map);
		jc::this_call(0x57ED80, ptr(g_weapon.get()) + 0x4, &mem);

		local_char->set_weapon(100, false);

		//g_factory->spawn_damageable_object(local_pos + vec3(2.f, 0.f, 0.f), "building_blocks\\general\\oil_barrel_red.lod", "models\\building_blocks\\general\\oil_barrel.pfx");

		//auto _char = info.character ? info.character : local_char;

		/*auto r = info.vehicle->get_driver_seat();

		log(RED, "{:x} {:x}", ptr(info.vehicle), ptr(r.obj));
	}*/

	static GamePlayer* npc_lp = nullptr;

	/*if (g_key->is_key_pressed(VK_NUMPAD7))
	{
		if (const auto veh = BITCAST(Vehicle*, g_global_ptr))
		{
			const auto seat = veh->get_special_seat();

			if (const auto weapon = seat->get_weapon())
			{
				weapon->set_last_shot_time(jc::nums::MAXF);
				weapon->force_fire();
				weapon->set_enabled(true);
				weapon->update();
			}
		}
	}*/

	static bool entered = false;

	if (info.handle)
	{
		auto t = ptr(info.character);

		if (const auto veh = BITCAST(Vehicle*, g_global_ptr); veh && !entered)
		{
			const auto seat = veh->get_driver_seat();

		}
	}

	/*if (g_key->is_key_pressed(VK_ADD))
	{
		
		if (!info.handle)
		{
			info.handle = g_factory->spawn_character("female1", g_world->get_local_character()->get_position());
			info.character = info.handle->get_character();

			log(CYAN, "handle {:x}", ptr(info.handle));
			log(CYAN, "handle base {:x}", ptr(info.handle->get_controller()));
			log(CYAN, "handle base from character {:x}", ptr(info.character->get_controller()));
			log(CYAN, "char {:x}", ptr(info.character));

			info.character->set_position(local_pos + vec3(-1.f, 0.f, 0.f));
			info.character->set_model(6);

			entered = false;

			if (const auto veh = BITCAST(Vehicle*, g_global_ptr))
			{
				const auto seat = veh->get_driver_seat();
				const auto interactable = seat->get_interactable();

				//seat->warp_character(info.character, true);

				interactable->interact_with(info.character);
				//seat->warp_character(info.character, true);
			}
			
			//info.character->set_body_stance(29);
			//info.character->set_body_stance(27);
			//info.character->set_body_stance(25);
			//info.character->set_body_stance(52);
		}
		else
		{
			g_factory->destroy_character_handle(info.handle);

			info.handle = nullptr;
			info.character = nullptr;
		}
	}*/

	static CharacterHandle* handle = nullptr;

	static AnimatedRigidObject* garage_door = nullptr;

	static TrafficLight* tl = nullptr;

	if (g_key->is_key_down(VK_NUMPAD1))
	{
		//local_char->set_walking_anim_set(4, 7);

		/*struct vel_test
		{
			vec3  dir;
			vec3  pos;
			float vel;
			float damage;
			float unk1, unk2;
			float min_vel;
		} ay;

		ay.dir = { 153.f, 159.f, 56.48f };
		ay.pos = local_pos;
		ay.vel = 0.f;
		ay.damage = 0.f;
		ay.unk1 = 1.f;
		ay.unk2 = 1.f;
		ay.min_vel = 100.f;

		vec3 test = vec3(10.f, 0.f, 0.f);

		//(*(int(__thiscall**)(ptr, vec3*))(*(ptr*)out[1] + 0x10))(out[1], &test);

		//(*(void(__thiscall**)(Character*, vel_test*))(*(ptr*)local_char + 0x7C))(local_char, &ay);

		auto physical = local_char->get_pfx();

		jc::this_call(0x744B20, physical.obj, &ay.dir, ptr(local_char) + 0x608);

		*(ptr*)(ptr(local_char) + 0x884) =  *(ptr*)(ptr(local_char) + 0x884) & 0xFFFBFFFF;*/

		/*auto variant = NPCVariant::CREATE();

		log(RED, "nice 1 {:x}", ptr(*variant));

		*(jc::stl::string*)(ptr(*variant) + 0xEC) = "characters\\paperdolls\\prop_bottle.lod";
		//*(jc::stl::string*)(ptr(*variant) + 0x15C) = "characters\\paperdolls\\PROP_policehat.lod";
		*(jc::stl::string*)(ptr(*variant) + 0x38C) = "attach_left";
		//*(jc::stl::string*)(ptr(*variant) + 0x3A8) = "attach_right";
		//*(jc::stl::string*)(ptr(*variant) + 0x3A8) = "head";
		//*(jc::stl::string*)(ptr(*variant) + 0x3C4) = "spine_1";
		//*(jc::stl::string*)(ptr(*variant) + 0x3E0) = "spine_2";
		//*(jc::stl::string*)(ptr(*variant) + 0x3FC) = "pelvis";
		//*(jc::stl::string*)(ptr(*variant) + 0x418) = "characters\\kc_007\\kc_007.lod";
		//*(jc::stl::string*)(ptr(*variant) + 0x434) = "ezperansa_bag_hide";
		//*(jc::stl::string*)(ptr(*variant) + 0x4C0) = "ezperansa_bag_show";

		log(RED, "nice 2");

		local_char->set_npc_variant(*variant);*/
	}

	static ptr anim_ptr = 0;
	static ptr anim		= 0;

	/*if (g_key->is_key_pressed(VK_NUMPAD3))
	{
		jc::stl::string anim_name = R"(Models\Characters\Animations\NPCMoves\hooker\dance_hooker_NPC_2.anim)";
		// std::string anim_name = R"(Models\Characters\Animations\NPCMoves\idle\smoke_npc_2.anim)";

		// if (!anim)
		{
			ptr dummy[2] = { 0 };

			anim_ptr = jc::this_call(0x55EE80, jc::read<void*>(0xD84D14), dummy, &anim_name);
			anim	 = *(ptr*)anim_ptr;

			// ptr dummy2[2] = { 0 };
			// jc::this_call<ptr>(0x55E9D0, jc::read<void*>(0xD84D14), dummy2, anim_name.c_str());
		}

		// local_char->set_animation(anim_name, 0.2f, true, true);
	}*/

	// rbm reversing

	/*if (g_key->is_key_pressed(VK_NUMPAD2))
	{
		using namespace enet;

		std::vector<uint8_t> out;

		auto rbm = util::fs::read_bin_file("crate_custom_png.rbm");

		const auto type = deserialize_string(rbm);
		const auto major = deserialize_int(rbm);
		const auto minor = deserialize_int(rbm);
		const auto minor2 = deserialize_int(rbm);
		const auto min_bb = deserialize_general_data<vec3>(rbm);
		const auto max_bb = deserialize_general_data<vec3>(rbm);
		const auto blocks_count = deserialize_int(rbm);

		serialize_params(out, type);
		serialize_params(out, major);
		serialize_params(out, minor);
		serialize_params(out, minor2);
		serialize_params(out, min_bb);
		serialize_params(out, max_bb);
		serialize_params(out, blocks_count);

		log(GREEN, "version: {}.{}.{}", major, minor, minor2);
		log(GREEN, "header: '{}'", type);

		for (int i = 0; i < blocks_count; ++i)
		{
			const auto block_type = deserialize_string(rbm);

			log(GREEN, "  block {} type: '{}'", i, block_type);

			serialize_params(out, block_type);

			int textures_count = 0;

			bool use_color = false;
			bool use_normals = false;

			switch (util::hash::JENKINS(block_type))
			{
			case util::hash::JENKINS("Diffuse"):
			{
				textures_count = 1;
				use_color = false;
				use_normals = true;
				break;
			}
			case util::hash::JENKINS("DiffuseVertexColors"):
			{
				textures_count = 1;
				use_color = true;
				use_normals = true;
				break;
			}
			case util::hash::JENKINS("AdditiveAlpha"):
			{
				textures_count = 1;
				use_color = false;
				use_normals = false;
				break;
			}
			default: log(RED, "    Unknown block type: {}", type);
			}

			for (int texture_i = 0; texture_i < textures_count; ++texture_i)
			{
				const auto texture_name = deserialize_string(rbm);

				serialize_params(out, texture_name);

				log(GREEN, "    texture {} name: '{}'", texture_i, texture_name);
			}

			for (int i = 0; i < 20; ++i)
			{
				const auto f = deserialize_float(rbm);

				serialize_params(out, f);
			}

			const auto primitive_type = deserialize_int(rbm);
			const auto vertex_count = deserialize_int(rbm);

			serialize_params(out, primitive_type);
			serialize_params(out, vertex_count);

			log(GREEN, "    primitive type: {}", primitive_type);
			log(GREEN, "    vertices count: {}", vertex_count);

			for (int v = 0; v < vertex_count; ++v)
			{
				const auto f = deserialize_general_data<vec3>(rbm);

				serialize_params(out, f);

				log(GREEN, "    vertex {}: {:.2f}, {:.2f}, {:.2f}", v, f.x, f.y, f.z);
			}

			const auto unk0 = deserialize_int<uint32_t>(rbm);

			log(GREEN, "    vertices infos: {}", unk0);

			serialize_params(out, unk0);

			for (int v = 0; v < vertex_count; ++v)
			{
				auto uv = deserialize_general_data<vec2>(rbm);

				//vec2 fixed[4] =
				//{
				//	{ 1.f, 1.f },
				//	{ 1.f, 0.f },
				//	{ 0.f, 1.f },
				//	{ 0.f, 0.f },
				//};

				//static int i = 0;

				//uv = fixed[i];

				//i = (i + 1) % 4;

				serialize_params(out, uv);

				if (use_color)
				{
					const auto color = deserialize_int<uint32_t>(rbm);

					serialize_params(out, color);

					log(GREEN, "    color {}: {:x}", v, color);
				}

				if (use_normals)
				{
					const auto _nx = deserialize_int<int8_t>(rbm);
					const auto _ny = deserialize_int<int8_t>(rbm);
					const auto _nz = deserialize_int<int8_t>(rbm);
					const auto _nw = deserialize_int<int8_t>(rbm);

					const auto nx = util::pack::unpack_float(_nx, 127.f);
					const auto ny = util::pack::unpack_float(_ny, 127.f);
					const auto nz = util::pack::unpack_float(_nz, 127.f);
					const auto nw = util::pack::unpack_float(_nw, 127.f);

					serialize_params(out, util::pack::pack_float<int8_t>(nx, 127.f));
					serialize_params(out, util::pack::pack_float<int8_t>(ny, 127.f));
					serialize_params(out, util::pack::pack_float<int8_t>(nz, 127.f));
					serialize_params(out, util::pack::pack_float<int8_t>(nw, 127.f));

					log(GREEN, "    normal {}: {:.2f} {:.2f} {:.2f}", v, nx, ny, nz);
				}

				log(GREEN, "    uv {}: {:.2f}, {:.2f}", v, uv.x, uv.y);
			}

			auto indices_count = deserialize_int(rbm);

			log(RED, "trash indices: {}", indices_count);

			serialize_params(out, indices_count);

			if (indices_count == 0)
			{
				indices_count = deserialize_int(rbm);

				serialize_params(out, indices_count);
			}

			log(GREEN, "    indices_count: {}", indices_count);

			for (int v = 0; v < indices_count; ++v)
			{
				const auto index = deserialize_int<uint16_t>(rbm);

				serialize_params(out, index);

				log(GREEN, "    index {}: {}", v, index);
			}

			const auto block_end = deserialize_int<uint32_t>(rbm);

			serialize_params(out, block_end);

			if (block_end == 0x89ABCDEF)
				log(CYAN, "    block ended");
		}

		std::ofstream test_file("test.rbm", std::ios::binary);

		test_file.write((char*)out.data(), out.size());
	}*/
}
#endif