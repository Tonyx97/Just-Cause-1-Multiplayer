#include <defs/standard.h>

#include "test_units.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/object/character/character.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/transform/transform.h>
#include <game/object/mounted_gun/mounted_gun.h>
#include <game/object/weapon/weapon.h>
#include <game/sys/all.h>

/*
* code where this int is set to progress in the level loading
* 
004980CD - C7 42 1C 01000000 - mov [edx+1C],00000001
00497B28 - C7 41 20 02000000 - mov [ecx+20],00000002
00497B59 - C7 41 20 03000000 - mov [ecx+20],00000003
00497BE1 - C7 41 20 04000000 - mov [ecx+20],00000004
*/

bool __fastcall hk_test(int a1, void*, int key)
{
	auto res = jc::hooks::call<jc::proto::dbg::test>(a1, key);

	//if (key == 0x5c)
	//if (res)
		//log(GREEN, "[1 - {:x}] {:x} -> {}", ptr(_ReturnAddress()), key, res);
	
	return res;
}

int __fastcall hk_test2(int m, void*, std::string* str)
{
	if (str)
		log(RED, "Requested '{}'", str->c_str());

	return jc::hooks::call<jc::proto::dbg::test2>(m, str);
}

void jc::test_units::init()
{
	//jc::hooks::hook<jc::proto::dbg::test>(&hk_test);
	jc::hooks::hook<jc::proto::dbg::test2>(&hk_test2);
}

void jc::test_units::destroy()
{
	jc::hooks::unhook<jc::proto::dbg::test2>();
	//jc::hooks::unhook<jc::proto::dbg::test>();
}

void jc::test_units::test_0()
{
	auto local_char = g_world->get_localplayer_character();

	if (!local_char)
		return;

	auto	  local_pos = local_char->get_position();
	Transform local_t(local_pos);

	if (g_key->is_key_pressed(VK_NUMPAD8))
	{
		g_spawn->spawn_damageable_object(local_pos + vec3(2.f, 0.f, 0.f), "building_blocks\\general\\oil_barrel_red.lod", "models\\building_blocks\\general\\oil_barrel.pfx");
		// g_spawn->spawn_simple_rigid_object(local_pos + vec3(2.f, 0.f, 0.f), "building_blocks\\general\\coca_pakage_box.lod", "coca_package_box.pfx");
	}

	if (g_key->is_key_pressed(VK_ADD))
	{
		g_day_cycle->set_night_time_enabled(false);
	}

	if (g_key->is_key_pressed(VK_NUMPAD9))
	{
		g_day_cycle->set_night_time_enabled(true);
	}

	static CharacterHandle* handle = nullptr;

	if (g_key->is_key_pressed(VK_NUMPAD2))
	{
		//handle = g_spawn->spawn_character("female1", local_pos + vec3(2.f, 0.f, 0.f), Weapon_Pistol);

		//log(RED, "handle {} (char {})", (void*)handle, (void*)handle->get_character());
	}

	static std::vector<ref<MountedGun>> temp_holder;

	if (g_key->is_key_pressed(VK_NUMPAD4))
	{
		//handle->destroy();

		auto rf = g_game_control->create_object<MountedGun>();

		if (rf)
		{
			g_game_control->enable_object(rf);

			log(GREEN, "Mounted gun created {:x}", ptr(rf.obj));

			object_base_map map {};

			map.insert<ValueType_String>(0x274e57fb, R"(Mounted M60)");
			map.insert<ValueType_Float>(0x71b9da94, -1.00f);
			map.insert<ValueType_Float>(0x9fcd3bc1, 1.00f);
			map.insert<ValueType_Float>(0x9b92cd11, -0.33f);
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
			map.insert<ValueType_Mat4>(0xACAEFB1, &local_t);
			//map.insert<ValueType_String>(0xA4AB5487, "weapons\\weap_011_lave_037_mount.lod");
			map.insert<ValueType_String>(0x3921ad5f, R"(weapons\weap_011_lave_037_mount.lod)");
			//map.insert<ValueType_String>(0xA4AB5487, "building_blocks\\general\\oil_barrel_red.lod");

			

			jc::this_call(0x645AA0, rf.obj, &map);

			temp_holder.push_back(std::move(rf));
		}
	}

	if (g_key->is_key_pressed(VK_NUMPAD7))
	{
		auto mem = jc::game::malloc<ptr>(0x120);

		jc::this_call(0x761700, mem);

		*(ptr*)(mem + 0xAC) = 2;

		std::string name = "exported\\agenttypes\\key_characters\\KEY_Kane_Bikini.ee";

		bool res = jc::this_call(0x5C0CE0, mem, &name);

		log(RED, "Request {:x}", ptr(mem));

		auto p = jc::read<ptr>(mem, 0x118);

		while (!jc::read<bool>(p, 0x8))
		{
			auto asset_loader = jc::read<ptr>(0xDA1C24);

			log(CYAN, "loading {:x}", p);

			jc::this_call(0x989640, asset_loader);

			SleepEx(0, true);
		}

		local_char->set_model("female1");

		//jc::v_call(mem, 3);
		//jc::v_call(mem, 2, 1);

		log(CYAN, "loaded");

		log(RED, "result: {}", res);
	}

	if (g_key->is_key_down(VK_NUMPAD1))
	{
		local_char->set_model("female1");
	}

	static ptr anim_ptr = 0;
	static ptr anim		= 0;

	if (g_key->is_key_pressed(VK_NUMPAD3))
	{
		std::string anim_name = R"(Models\Characters\Animations\NPCMoves\hooker\dance_hooker_NPC_2.anim)";
		// std::string anim_name = R"(Models\Characters\Animations\NPCMoves\idle\smoke_npc_2.anim)";

		// if (!anim)
		{
			ptr dummy[2] = { 0 };

			anim_ptr = jc::this_call<ptr>(0x55EE80, jc::read<void*>(0xD84D14), dummy, &anim_name);
			anim	 = *(ptr*)anim_ptr;

			// ptr dummy2[2] = { 0 };
			// jc::this_call<ptr>(0x55E9D0, jc::read<void*>(0xD84D14), dummy2, anim_name.c_str());
		}

		/*while (!GetAsyncKeyState(VK_F3))
			Sleep(100);*/

		// local_char->set_animation(anim_name, 0.2f, true, true);
	}

	int a = 0;

	if (g_key->is_key_pressed(VK_NUMPAD5))
	{
		std::string anim_name = R"(dance_hooker_NPC_2.anim)";

		/*ptr temp = 0;

		jc::this_call<bool>(0x659FF0, jc::read<ptr>(0xD84D14) + 0x4, &temp, &anim_name);

		log(RED, "anim {:x} {:x}", anim, temp);

		log(RED, "loaded: {}", temp != jc::read<ptr>(jc::read<ptr>(0xD84D14) + 0x8));*/

		local_char->set_animation(anim_name, 0.2f, true, true);
	}
}