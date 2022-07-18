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
#include <game/object/sound/sound_game_obj.h>
#include <game/object/mission/objective.h>
#include <game/object/ui/map_icon.h>
#include <game/sys/all.h>

/*
* code where this int is set to progress in the level loading
* 
004980CD - C7 42 1C 01000000 - mov [edx+1C],00000001
00497B28 - C7 41 20 02000000 - mov [ecx+20],00000002
00497B59 - C7 41 20 03000000 - mov [ecx+20],00000003
00497BE1 - C7 41 20 04000000 - mov [ecx+20],00000004
*/

std::set<ptr> ay;

void __fastcall hk_test(ptr event_list, void*, ptr userdata)
{
	if (!ay.contains(event_list))
	{
		ay.insert(event_list);
		log(CYAN, "added {:x} {:x} - {:x}", event_list, userdata, ptr(_ReturnAddress()));
	}

	//jc::hooks::call<jc::proto::dbg::test>(event_list, userdata);
}

void __fastcall hk_test2(ptr a1, void*, int id)
{
	//jc::hooks::call<jc::proto::dbg::test2>(a1, id);
}

void jc::test_units::init()
{
	//jc::hooks::hook<jc::proto::dbg::test>(&hk_test);
	//jc::hooks::hook<jc::proto::dbg::test2>(&hk_test2);
}

void jc::test_units::destroy()
{
	//jc::hooks::unhook<jc::proto::dbg::test2>();
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
		g_factory->spawn_damageable_object(local_pos + vec3(2.f, 0.f, 0.f), "building_blocks\\general\\oil_barrel_red.lod", "models\\building_blocks\\general\\oil_barrel.pfx");
		// g_factory->spawn_simple_rigid_object(local_pos + vec3(2.f, 0.f, 0.f), "building_blocks\\general\\coca_pakage_box.lod", "coca_package_box.pfx");
	}

	if (g_key->is_key_pressed(VK_ADD))
	{
		g_factory->create_objective(local_pos + vec3(20.f, 0.f, 0.f), { util::rand::rand_int(0, 255), util::rand::rand_int(0, 255), util::rand::rand_int(0, 255), 255 });

		/*static int i = 0;

		i = ++i % 16;

		g_factory->create_map_icon(local_pos + vec3(20.f, 0.f, 0.f), i);*/
	}

	static CharacterHandle* handle = nullptr;

	static AnimatedRigidObject* garage_door = nullptr;

	if (g_key->is_key_pressed(VK_NUMPAD2))
	{
		//handle = g_factory->spawn_character("female1", local_pos + vec3(2.f, 0.f, 0.f), Weapon_Pistol);

		//log(RED, "handle {} (char {})", (void*)handle, (void*)handle->get_character());

		/*for (const auto& t : temp_vec)
		{
			jc::write<uint16_t>(jc::game::float_to_i16(1.f), *t, 0x130);
			jc::this_call(0x783250, *t, jc::this_call<float>(0x7845B0, *t));
		}*/

		garage_door->call_event(0x284);
	}

	if (g_key->is_key_pressed(VK_NUMPAD9))
	{
		garage_door->call_event(0x288);
	}

	if (g_key->is_key_pressed(VK_NUMPAD4))
	{
		if (garage_door = g_factory->spawn_animated_rigid_object(
			local_pos,
			R"(building_blocks\general\safehouse_guer_garage_door.lod)",
			R"(models\building_blocks\general\safehouse_guer_garage_door_col.pfx)"))
		{
			log(RED, "nice {:x}", ptr(garage_door));

			garage_door->setup_event_and_subscribe(0x290, "custom::anim_end");
			garage_door->setup_event_and_subscribe(0x284, "custom::opendoor");
			garage_door->setup_event_and_subscribe(0x288, "custom::closedoor");

			const auto open_sound = g_sound->create_sound(local_pos, "door\\door_garage", 0);
			const auto close_sound = g_sound->create_sound(local_pos, "door\\door_garage", 1);

			open_sound->setup_event_and_subscribe(0xB0, "custom::opendoor");

			close_sound->setup_event_and_subscribe(0xA8, "custom::closedoor");
			close_sound->setup_event_and_subscribe(0xAC, "custom::opendoor");
			close_sound->setup_event_and_subscribe(0xB0, "custom::anim_end");
		}
	}

	if (g_key->is_key_pressed(VK_NUMPAD7))
	{
		/*static int id = 0;

		local_char->set_model(id++);

		if (id >= jc::vars::exported_entities.size())
			id = 0;*/

		local_char->set_model(7);
	}

	if (g_key->is_key_pressed(VK_NUMPAD1))
	{
		struct vel_test
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

		*(ptr*)(ptr(local_char) + 0x884) =  *(ptr*)(ptr(local_char) + 0x884) & 0xFFFBFFFF;

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
		jc::stl::string anim_name = R"(std_drink_rum_NPC.anim)";
		//std::string anim_name = R"(dance_hooker_NPC_2.anim)";

		/*ptr temp = 0;

		jc::this_call<bool>(0x659FF0, jc::read<ptr>(0xD84D14) + 0x4, &temp, &anim_name);

		log(RED, "anim {:x} {:x}", anim, temp);

		log(RED, "loaded: {}", temp != jc::read<ptr>(jc::read<ptr>(0xD84D14) + 0x8));*/

		local_char->set_animation(anim_name, 0.2f, true, true);
	}
}