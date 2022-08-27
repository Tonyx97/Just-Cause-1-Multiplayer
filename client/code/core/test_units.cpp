#include <defs/standard.h>

#include "test_units.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/transform/transform.h>
#include <game/object/character/character.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/mounted_gun/mounted_gun.h>
#include <game/object/item/item_pickup.h>
#include <game/object/weapon/weapon.h>
#include <game/object/resource/ee_resource.h>
#include <game/object/exported_entity/exported_entity.h>
#include <game/object/agent_type/vehicle_type.h>
#include <game/object/agent_type/npc_variant.h>
#include <game/object/rigid_object/animated_rigid_object.h>
#include <game/object/rigid_object/traffic_light.h>
#include <game/object/character/comps/vehicle_controller.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/sound/sound_game_obj.h>
#include <game/object/mission/objective.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/vehicle/comps/vehicle_seat.h>
#include <game/object/interactable/interactable.h>
#include <game/object/ui/map_icon.h>
#include <game/object/localplayer/localplayer.h>
#include <game/object/physics/pfx_collision.h>
#include <game/object/physics/pfx_base.h>
#include <game/object/base/comps/physical.h>
#include <game/sys/all.h>

#include <net/serializer.h>

// 40E940 = FnThatReadsAssetFromDisk

DEFINE_HOOK_THISCALL(resource_request, 0x5C2DC0, int, ptr a1, jc::stl::string* name, int type, ptr data, ptr size)
{
	if (strstr(name->c_str(), "kc_022_lod1.rbm"))
	{
		/*log(RED, "{:x} {:x} {:x} {}", a1, data, size, name->c_str());

		const auto data_file = util::fs::read_bin_file("kc_022_lod1.rbm");

		size = data_file.size();

		log(RED, "size: {}", data_file.size());

		memcpy((void*)data, data_file.data(), size);

		log(RED, "size: {}", data_file.size());*/
	}

	//while (!GetAsyncKeyState(VK_F3));

	return resource_request_hook.call(a1, name, type, data, size);
}

DEFINE_HOOK_THISCALL(_test1, 0x93DE90, void, int _this, int a2)
{
	_test1_hook.call(_this, a2);

	log(RED, "{}", jc::read<float>(_this, 0xC));

	/*if (jc::read<float>(_this, 0x10) > 2000.f)
	{
		jc::write<float>(99999.f, _this, 0x10);
		jc::write<float>(99999.f, _this, 0xC);
	}*/
}

DEFINE_HOOK_THISCALL_S(_test2, 0x597B80, bool, int _this)
{
	auto res = _test2_hook.call(_this);

	/*if (res && _this != ptr(g_world->get_localplayer_character()))
		log(RED, "2 {:x} {:x}", RET_ADDRESS, _this);*/

	return res;
}

DEFINE_HOOK_THISCALL_S(_test3, 0x596420, bool, int _this)
{
	auto res = _test3_hook.call(_this);

	/*if (res && _this != ptr(g_world->get_localplayer_character()))
		log(RED, "3 {:x} {:x}", RET_ADDRESS, _this);*/

	return res;
}

DEFINE_HOOK_THISCALL_S(_test4, 0x59A560, bool, int _this)
{
	auto res = _test4_hook.call(_this);

	if (res && _this != ptr(g_world->get_localplayer_character()))
		log(RED, "4 {:x} {:x}", RET_ADDRESS, _this);

	return res;
}


void jc::test_units::init()
{
	//_test1_hook.hook();
	/*_test2_hook.hook();
	_test3_hook.hook();
	_test4_hook.hook();*/

	//_set_boat_vel_hook.hook();
	//resource_request_hook.hook();
	//_test_hook.hook();
}

void jc::test_units::destroy()
{
	_test1_hook.unhook();
	/*_test2_hook.unhook();
	_test3_hook.unhook();
	_test4_hook.unhook();*/

	//_set_boat_vel_hook.unhook();
	//resource_request_hook.unhook();
	//_test_hook.unhook();
}

void jc::test_units::test_0()
{
	auto localplayer = g_world->get_localplayer();

	auto local_char = g_world->get_localplayer_character();

	if (!local_char)
		return;

	auto local_pos = local_char->get_position();
	Transform local_t(local_pos);

	static std::vector<ref<Vehicle>> vehs;

	if (g_key->is_key_pressed(VK_NUMPAD9))
	{
		//jc::this_call(0x4CE770, localplayer, g_player_global_info->get_local_controller());

		auto veh = BITCAST(Vehicle*, g_global_ptr);

		const auto seat = veh->get_driver_seat();

		//jc::this_call(0x74DE20, *seat, true);

		//const auto interactable = seat->get_interactable();
		//interactable->interact_with(local_char);

		//g_factory->spawn_simple_rigid_object(local_pos + vec3(2.f, 0.f, 0.f), "crate_custom_png.rbm", "");
		//g_archives->dump_hashed_assets();
		/*const auto asset = g_archives->get_asset(R"(PROP_ponytailgrey.lod)");
		log(GREEN, "{} {:x} {:x} {:x}", asset.arc_index, asset.hash, asset.offset, asset.size);*/
	}

	if (g_key->is_key_pressed(VK_NUMPAD4))
	{
		std::string model = "mim.rbm"; // "crate_custom_png.rbm"

		g_texture_system->load_texture("mim.jpg");
		g_model_system->load_rbm(model);

		g_factory->spawn_simple_rigid_object(local_pos + vec3(2.f, 0.f, 0.f), model, "");

		g_model_system->unload_rbm(model);
		g_texture_system->unload_texture("mim.dds");
	}

	//if (auto entry = g_archives->get_asset_entry(R"(E:\SteamLibrary\steamapps\common\Just Cause\Models\Characters\Animations\NPCMoves\hooker\dance_hooker_NPC_1.anim)"))
	//	log(YELLOW, "{:x} {:x} {:x}", entry->hash, entry->offset, entry->size);

	if (g_key->is_key_pressed(VK_NUMPAD5))
	{
		g_anim_system->load_anim("test.anim");
		local_char->set_animation("test.anim", 0.2f, true, true);
		g_anim_system->unload_anim("test.anim");

		//local_char->set_animation("dance_hooker_NPC_2.anim", 0.2f, true, true);
	}

	struct TestInfo
	{
		CharacterHandle* handle = nullptr;
		Character* character = nullptr;
		Vehicle* vehicle = nullptr;
	} static info;

	if (auto veh_controller = local_char->get_vehicle_controller())
		if (auto veh = veh_controller->get_vehicle())
			info.vehicle = veh;

	if (g_key->is_key_pressed(VK_F8))
	{
		if (info.character)
		{
			g_factory->destroy_character_handle(info.handle);

			info.handle = nullptr;
			info.character = nullptr;
		}
	}

	if (g_key->is_key_pressed(VK_NUMPAD8))
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

		if (r && _char)
		{
			r->warp_character(_char);

			//info.vehicle->set_velocity(vec3(0.f, 10.f, 0.f));

			log(GREEN, "warped? {:x}", jc::read<ptr>(ptr(*r), 0x48));
		}*/
	}

	static LocalPlayer* npc_lp = nullptr;

	if (g_key->is_key_pressed(VK_NUMPAD7))
	{
		if (const auto veh = BITCAST(Vehicle*, g_global_ptr))
		{
			const auto seat = veh->get_driver_seat();

			seat->kick_current(true);

			/*if (local_char->get_vehicle())
				seat->kick_current(true);
			else seat->warp_character(local_char, true);*/

			/*const auto sound_comp = jc::read<ptr>(veh, 0x404);

			auto res = (*(int(__thiscall**)(ptr, int, ptr))(**(ptr**)(sound_comp + 0xC0) + 0x30))(
				*(ptr*)(sound_comp + 0xC0),
				5,
				*(ptr*)(sound_comp + 4));

			if (res)
			{
				if ((*(int(__thiscall**)(int))(*(ptr*)res + 0x6C))(res))
				{
					ptr v3 = (*(ptr(__thiscall**)(int))(*(ptr*)res + 0x6C))(res);
					(*(void(__thiscall**)(int, int))(*(ptr*)v3 + 0x10))(v3, sound_comp + 0x50);
					(*(int(__thiscall**)(int))(*(ptr*)res + 0x18))(res);

					log(GREEN, "played");
				}
			}*/
		}
	}

	static bool entered = false;

	if (info.handle)
	{
		auto t = ptr(info.character);

		if (const auto veh = BITCAST(Vehicle*, g_global_ptr); veh && !entered)
		{
			const auto seat = veh->get_driver_seat();
			const auto seat_ptr = ptr(*seat);

			//seat->open_door(info.character);

			if (seat->get_character() == info.character)
			{
				if (!jc::this_call(0x5A1F30, info.character))
				{
					log(BLUE, "Part 2");

					*(uint8_t*)(seat_ptr + 0x152) = 0;

					const auto type = seat->get_type();

					if (type != VehicleSeat_Special)
					{
						if (type == VehicleSeat_Passenger)
							info.character->set_stance_enter_vehicle_right(false);
						else if (type == VehicleSeat_Driver)
							info.character->set_stance_enter_vehicle_left(false);
					}

					entered = true;
				}
				else log(BLUE, "if needed, code is at 0x88102A");
			}
		}
	}

	if (g_key->is_key_pressed(VK_ADD))
	{
		/*if (!npc_lp)
		{
			npc_lp = game::malloc<LocalPlayer>(0x53C);

			jc::this_call(0x4C03B0, npc_lp);

			log(GREEN, "npc lp: {:x} {:x}", ptr(npc_lp), ptr(localplayer));
		}*/
		
		if (!info.handle)
		{
			info.handle = g_factory->spawn_character("female1", g_world->get_localplayer_character()->get_position());
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

			/*
			jc::write(info.character, npc_lp, 0x1C);
			
			if (auto weapon = info.character->get_weapon_belt()->add_weapon(Weapon_2H_SMG)) // Weapon_1H_SMG - Weapon_Grenade_Launcher
			{
				info.character->set_draw_weapon(weapon);
				info.character->apply_weapon_switch();
			}*/
		}
		else
		{
			g_factory->destroy_character_handle(info.handle);

			info.handle = nullptr;
			info.character = nullptr;
		}
	}
	
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

		auto physical = local_char->get_physical();

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

	if (g_key->is_key_pressed(VK_NUMPAD3))
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

		/*while (!GetAsyncKeyState(VK_F3))
			Sleep(100);*/

		// local_char->set_animation(anim_name, 0.2f, true, true);
	}



	// rbm reversing

	if (g_key->is_key_pressed(VK_NUMPAD2))
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

				/*vec2 fixed[4] =
				{
					{ 1.f, 1.f },
					{ 1.f, 0.f },
					{ 0.f, 1.f },
					{ 0.f, 0.f },
				};

				static int i = 0;

				uv = fixed[i];

				i = (i + 1) % 4;*/

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
	}
}