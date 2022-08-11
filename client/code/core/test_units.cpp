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
#include <game/object/agent_type/npc_variant.h>
#include <game/object/rigid_object/animated_rigid_object.h>
#include <game/object/rigid_object/traffic_light.h>
#include <game/object/character/comps/vehicle_controller.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/sound/sound_game_obj.h>
#include <game/object/mission/objective.h>
#include <game/object/vehicle/vehicle.h>
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

void jc::test_units::test_0()
{
	auto localplayer = g_world->get_localplayer();

	auto local_char = g_world->get_localplayer_character();

	if (!local_char)
		return;

	auto local_pos = local_char->get_position();
	Transform local_t(local_pos);

	// icon stuff

	static std::vector<ref<ptr>> ay;
	static std::vector<bref<ptr>> ay2;
	static std::vector<ref<ptr>> ay3;

	if (g_key->is_key_pressed(VK_NUMPAD9))
	{
		g_factory->spawn_simple_rigid_object(local_pos + vec3(2.f, 0.f, 0.f), "crate_custom_png.rbm", "");

		//g_archives->dump_hashed_assets();

		/*const auto asset = g_archives->get_asset(R"(PROP_ponytailgrey.lod)");

		log(GREEN, "{} {:x} {:x} {:x}", asset.arc_index, asset.hash, asset.offset, asset.size);*/
	}

	if (g_key->is_key_down(VK_NUMPAD4))
	{
		jc::stl::string anim_name = "test2.anim";

		g_rsrc_streamer->load_texture("texture.png");
		g_rsrc_streamer->load_rbm("crate_custom_png.rbm");
		g_rsrc_streamer->load_pfx("crate.pfx");

		g_factory->spawn_simple_rigid_object(local_pos + vec3(2.f, 0.f, 0.f), "crate_custom_png.rbm", "crate.pfx");

		g_rsrc_streamer->unload_pfx("crate.pfx");
		g_rsrc_streamer->unload_rbm("crate_custom_png.rbm");
		g_rsrc_streamer->unload_texture("texture.png");
	}

	//if (auto entry = g_archives->get_asset_entry(R"(E:\SteamLibrary\steamapps\common\Just Cause\Models\Characters\Animations\NPCMoves\hooker\dance_hooker_NPC_1.anim)"))
	//	log(YELLOW, "{:x} {:x} {:x}", entry->hash, entry->offset, entry->size);

	if (g_key->is_key_pressed(VK_NUMPAD5))
	{
		g_rsrc_streamer->load_anim("test.anim");
		local_char->set_animation("test.anim", 0.2f, true, true);
		g_rsrc_streamer->unload_anim("test.anim");

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

	if (g_key->is_key_pressed(VK_ADD))
	{
		if (!info.handle)
		{
			info.handle = g_factory->spawn_character("female1", g_world->get_localplayer_character()->get_position());
			info.character = info.handle->get_character();

			log(CYAN, "handle {:x}", ptr(info.handle));
			log(CYAN, "handle base {:x}", ptr(info.handle->get_base()));
			log(CYAN, "handle base from character {:x}", ptr(info.character->get_handle_base()));
			log(CYAN, "char {:x}", ptr(info.character));

			info.character->set_model(6);

			/*if (auto weapon = info.character->get_weapon_belt()->add_weapon(Weapon_2H_SMG)) // Weapon_1H_SMG - Weapon_Grenade_Launcher
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