#include <defs/standard.h>

#include "test_units.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/transform/transform.h>
#include <game/object/character/character.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/mounted_gun/mounted_gun.h>
#include <game/object/weapon/weapon.h>
#include <game/object/resource/ee_resource.h>
#include <game/object/agent_type/npc_variant.h>
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

	log(WHITE, "--------------------------");
	for (int i = 0xC; i < 0x530; i += 0x1C)
		log(GREEN, "{:x} -> {}", i, ((std::string*)(a1 + i))->c_str());
	log(WHITE, "--------------------------");
	
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
	jc::hooks::hook<jc::proto::dbg::test>(&hk_test);
	//jc::hooks::hook<jc::proto::dbg::test2>(&hk_test2);
}

void jc::test_units::destroy()
{
	//jc::hooks::unhook<jc::proto::dbg::test2>();
	jc::hooks::unhook<jc::proto::dbg::test>();
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

		//g_spawn->spawn_mounted_gun(local_pos);
	}

	if (g_key->is_key_pressed(VK_NUMPAD7))
	{
		static int id = 0;

		local_char->set_model(id++);

		if (id >= jc::vars::exported_entities.size())
			id = 0;
	}

	if (g_key->is_key_pressed(VK_NUMPAD1))
	{
		auto variant = NPCVariant::CREATE();

		log(RED, "nice 1 {:x}", ptr(*variant));

		*(jc::stl::string*)(ptr(*variant) + 0xEC) = "characters\\paperdolls\\PROP_policehat.lod";
		*(jc::stl::string*)(ptr(*variant) + 0x15C) = "characters\\paperdolls\\prop_bottle.lod";
		*(jc::stl::string*)(ptr(*variant) + 0x38C) = "attach_right";
		*(jc::stl::string*)(ptr(*variant) + 0x3A8) = "attach_left";
		//*(jc::stl::string*)(ptr(*variant) + 0x3A8) = "head";
		//*(jc::stl::string*)(ptr(*variant) + 0x3C4) = "spine_1";
		//*(jc::stl::string*)(ptr(*variant) + 0x3E0) = "spine_2";
		//*(jc::stl::string*)(ptr(*variant) + 0x3FC) = "pelvis";
		//*(jc::stl::string*)(ptr(*variant) + 0x418) = "characters\\kc_007\\kc_007.lod";
		//*(jc::stl::string*)(ptr(*variant) + 0x434) = "ezperansa_bag_hide";
		//*(jc::stl::string*)(ptr(*variant) + 0x4C0) = "ezperansa_bag_show";

		log(RED, "nice 2");

		local_char->set_npc_variant(*variant);
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
		std::string anim_name = R"(std_drink_rum_NPC.anim)";
		//std::string anim_name = R"(dance_hooker_NPC_2.anim)";

		/*ptr temp = 0;

		jc::this_call<bool>(0x659FF0, jc::read<ptr>(0xD84D14) + 0x4, &temp, &anim_name);

		log(RED, "anim {:x} {:x}", anim, temp);

		log(RED, "loaded: {}", temp != jc::read<ptr>(jc::read<ptr>(0xD84D14) + 0x8));*/

		local_char->set_animation(anim_name, 0.2f, true, true);
	}
}