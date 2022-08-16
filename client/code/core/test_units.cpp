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
#include <game/object/agent_type/npc_variant.h>
#include <game/object/rigid_object/animated_rigid_object.h>
#include <game/object/rigid_object/traffic_light.h>
#include <game/object/character/comps/vehicle_controller.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/sound/sound_game_obj.h>
#include <game/object/mission/objective.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/vehicle/comps/vehicle_seat.h>
#include <game/object/ui/map_icon.h>
#include <game/object/localplayer/localplayer.h>
#include <game/object/physics/pfx_collision.h>
#include <game/object/physics/pfx_base.h>
#include <game/sys/all.h>

DEFINE_HOOK_THISCALL(_is_key_pressed, 0x48C850, bool, int _this, int key)
{
	const auto res = _is_key_pressed_hook.call(_this, key);

	if (res)
		log(RED, "[KEY PRESS] {:x} -> {} from {:x}", key, res, ptr(_ReturnAddress()));

	return res;
}

DEFINE_HOOK_THISCALL(_is_key_down, 0x48C800, bool, int _this, int key)
{
	const auto res = _is_key_down_hook.call(_this, key);

	if (res && key != 0x24 && key != 0x26 && key != 0x27)
		log(RED, "[KEY DOWN] {:x} -> {} from {:x}", key, res, ptr(_ReturnAddress()));

	return res;
}

DEFINE_HOOK_CCALL(_test, 0x407FD0, void, ptr a1, ptr a2)
{
	_test_hook.call(a1, a2);
}

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

/*DEFINE_HOOK_THISCALL(resource_request, 0x40E940, bool, ptr a1, ptr a2, ptr a3, ptr a4, ptr a5, ptr a6, ptr a7)
{
	log(RED, "{:x} {:x} {:x} {:x} {:x} {:x} {:x}", a1, a2, a3, a4, a5, a6, a7);

	auto res = resource_request_hook.call(a1, a2, a3, a4, a5, a6, a7);

	//while (!GetAsyncKeyState(VK_F3));

	return res;
}*/

/*DEFINE_HOOK_THISCALL(resource_request, 0x40EB80, bool, ptr a1, jc::stl::string* a2)
{
	log(RED, "{:x} {}", a1, a2->c_str());

	auto res = resource_request_hook.call(a1, a2);

	return res;
}*/

/*DEFINE_HOOK_STDCALL(resource_request, 0x76C83130, HANDLE,
	          LPCSTR                lpFileName,
	           DWORD                 dwDesiredAccess,
	          DWORD                 dwShareMode,
	 LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	           DWORD                 dwCreationDisposition,
	           DWORD                 dwFlagsAndAttributes,
	 HANDLE                hTemplateFile)
{
	log(RED, "{}", lpFileName);

	auto res = resource_request_hook.call(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	return res;
}*/

void jc::test_units::init()
{
	resource_request_hook.hook();
	//_is_key_pressed_hook.hook();
	//_is_key_down_hook.hook();
	//_test_hook.hook();
}

void jc::test_units::destroy()
{
	resource_request_hook.unhook();
	//_is_key_pressed_hook.unhook();
	//_is_key_down_hook.unhook();
	//_test_hook.unhook();
}

#include <net/serializer.h>

void jc::test_units::test_0()
{
	auto localplayer = g_world->get_localplayer();

	auto local_char = g_world->get_localplayer_character();

	if (!local_char)
		return;

	auto local_pos = local_char->get_position();
	Transform local_t(local_pos);

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

	static std::vector<ref<Vehicle>> vehs;

	if (g_key->is_key_pressed(VK_NUMPAD9))
	{
		g_rsrc_streamer->request_exported_entity(10, [&, local_pos, local_char](ExportedEntityResource* eer)
		{
			const auto ee = eer->get_exported_entity();

			std::string class_name;

			object_base_map* _map = nullptr;

			if (ee->take_class_property(&class_name, _map))
			{

				log(GREEN, "class name: {}", class_name);

				jc::stl::string _class_name = class_name;
				jc::stl::string _ee_name = jc::vars::exported_entities_vehicles[10];

				auto vtype = jc::c_call(0x671B40, 0x1F8, 0xA);

				log(RED, "vehicle type: {:x}", ptr(vtype));
				jc::this_call(0x817370, vtype, 1);
				jc::this_call(0x818350, vtype, &_class_name, &_ee_name, _map);

				_map->walk();

				Transform new_t(local_pos + vec3(3.f, 2.f, 0.f));

				log(RED, "{} {} {}", local_pos.x, local_pos.y, local_pos.z);

				ref<Vehicle> r;

				jc::this_call(0x8184E0, vtype, &r, &new_t, 0);

				log(RED, "vehicle created: {:x}", ptr(r.obj));

				(*(void(__thiscall**)(ptr, int))(ptr(*(ptr*)r.obj) + 0x28))(ptr(r.obj), 1);
				(*(void(__thiscall**)(ptr))(ptr(*(ptr*)r.obj) + 0x128))(ptr(r.obj));

				log(RED, " fucking local char {:x}", ptr(local_char));

				auto driver_seat = r.obj->get_driver_seat();

				driver_seat->warp_character(local_char);

				g_game_control->enable_object(r);

				vehs.push_back(std::move(r));
			}

			/*

			((ObjectBase*)vtype)->init_from_map(&map);

			jc::this_call(0x818350, vtype, &class_name, &ee_name, &map);

			ref<ptr> r;

			jc::this_call(0x8184E0, vtype, &r, &local_t, 1);

			log(RED, "vehicle created: {:x}", ptr(r.obj));

			vehs.push_back(std::move(r));*/
		}, true);

		//g_factory->spawn_simple_rigid_object(local_pos + vec3(2.f, 0.f, 0.f), "crate_custom_png.rbm", "");
		//g_archives->dump_hashed_assets();
		/*const auto asset = g_archives->get_asset(R"(PROP_ponytailgrey.lod)");
		log(GREEN, "{} {:x} {:x} {:x}", asset.arc_index, asset.hash, asset.offset, asset.size);*/
	}

	if (g_key->is_key_pressed(VK_NUMPAD4))
	{
		std::string model = "test.rbm"; // "crate_custom_png.rbm"

		g_texture_system->load_texture("1.jpg");
		g_model_system->load_rbm(model);
		g_physics->load_pfx("crate.pfx");

		g_factory->spawn_simple_rigid_object(local_pos + vec3(2.f, 0.f, 0.f), model, "crate.pfx");

		g_physics->unload_pfx("crate.pfx");
		g_model_system->unload_rbm(model);
		g_texture_system->unload_texture("1.jpg");

		const auto v = g_factory->create_map_icon("player_blip", jc::character::g::DEFAULT_SPAWN_LOCATION);

		log(GREEN, "blip: {:x}", ptr(v));
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

	if (g_key->is_key_pressed(VK_NUMPAD8) && info.vehicle)
	{
		//g_factory->spawn_damageable_object(local_pos + vec3(2.f, 0.f, 0.f), "building_blocks\\general\\oil_barrel_red.lod", "models\\building_blocks\\general\\oil_barrel.pfx");

		auto _char = info.character ? info.character : local_char;

		auto r = info.vehicle->get_driver_seat();

		log(RED, "{:x} {:x}", ptr(info.vehicle), ptr(r.obj));

		if (r && _char)
		{
			r->warp_character(_char);

			//info.vehicle->set_velocity(vec3(0.f, 10.f, 0.f));

			log(GREEN, "warped? {:x}", jc::read<ptr>(ptr(*r), 0x48));
		}
	}

	static LocalPlayer* npc_lp = nullptr;

	if (info.handle && npc_lp)
	{
		jc::this_call(0x4CB8C0, npc_lp);
	}

	if (g_key->is_key_pressed(VK_ADD))
	{
		if (!npc_lp)
		{
			npc_lp = game::malloc<LocalPlayer>(0x53C);

			jc::this_call(0x4C03B0, npc_lp);

			log(GREEN, "npc lp: {:x} {:x}", ptr(npc_lp), ptr(localplayer));
		}
		
		if (!info.handle)
		{
			info.handle = g_factory->spawn_character("female1", g_world->get_localplayer_character()->get_position());
			info.character = info.handle->get_character();

			log(CYAN, "handle {:x}", ptr(info.handle));
			log(CYAN, "handle base {:x}", ptr(info.handle->get_base()));
			log(CYAN, "handle base from character {:x}", ptr(info.character->get_handle_base()));
			log(CYAN, "char {:x}", ptr(info.character));

			info.character->set_position(local_pos + vec3(0.f, 50.f, 0.f));

			info.character->set_model(6);
			
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

	if (g_key->is_key_pressed(VK_NUMPAD7))
	{
		//tl->set_light(TrafficLight_Red);

		//g_weapon->dump();

		//jc::this_call(0x5A13B0, info.character);

		//local_char->set_body_stance(29);
		//local_char->set_body_stance(30);
		//local_char->set_added_velocity({ 0.f, 100.f, 0.f });

		log(RED, "called");
	}

	if (g_key->is_key_pressed(VK_NUMPAD1))
	{
		local_char->set_walking_anim_set(4, 7);

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
}