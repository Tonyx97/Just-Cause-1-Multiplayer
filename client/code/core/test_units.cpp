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
#include <game/sys/all.h>

/*
* code where this int is set to progress in the level loading
* 
004980CD - C7 42 1C 01000000 - mov [edx+1C],00000001
00497B28 - C7 41 20 02000000 - mov [ecx+20],00000002
00497B59 - C7 41 20 03000000 - mov [ecx+20],00000003
00497BE1 - C7 41 20 04000000 - mov [ecx+20],00000004
*/

bool __fastcall hk_test(int a1, void*, int a2, mat4* a3)
{
	auto res = jc::hooks::call<jc::proto::dbg::test>(a1, a2, a3);

	log(RED, "nice {:x} {:x}", a1, a2);
	
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
		g_spawn->spawn_damageable_object(local_pos + vec3(2.f, 0.f, 0.f), "building_blocks\\general\\oil_barrel_red.lod", "models\\building_blocks\\general\\oil_barrel.pfx");
		// g_spawn->spawn_simple_rigid_object(local_pos + vec3(2.f, 0.f, 0.f), "building_blocks\\general\\coca_pakage_box.lod", "coca_package_box.pfx");
	}

	if (g_key->is_key_pressed(VK_ADD))
	{
	}

	class SoundGameObject : public ObjectBase
	{
	public:

		IMPL_OBJECT_TYPE_ID("CSoundGameObject");
	};

	static CharacterHandle* handle = nullptr;

	static std::vector<ref<AnimatedRigidObject>> temp_vec;
	static std::vector<ref<SoundGameObject>> temp_vec2;

	if (g_key->is_key_pressed(VK_NUMPAD2))
	{
		//handle = g_spawn->spawn_character("female1", local_pos + vec3(2.f, 0.f, 0.f), Weapon_Pistol);

		//log(RED, "handle {} (char {})", (void*)handle, (void*)handle->get_character());

		/*for (const auto& t : temp_vec)
		{
			jc::write<uint16_t>(jc::game::float_to_i16(1.f), *t, 0x130);
			jc::this_call(0x783250, *t, jc::this_call<float>(0x7845B0, *t));
		}*/

		for (const auto& t : temp_vec)
		{
			t->get_event_manager()->call_event(0x284, &local_t);
		}
	}

	if (g_key->is_key_pressed(VK_NUMPAD9))
	{
		for (const auto& t : temp_vec)
		{
			t->get_event_manager()->call_event(0x288, &local_t);
		}
	}

	if (g_key->is_key_pressed(VK_NUMPAD4))
	{
		if (auto test = g_game_control->create_object<AnimatedRigidObject>())
		{
			{
				object_base_map map{};

				const auto m0 = mat4{ -1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.56f, 1.00f, 0.00f, 1.00f };
				map.insert<object_base_map::Float>(0x49985996, 0.00f); // float
				map.insert<object_base_map::Float>(0x1d39cbef, 5.f); // float
				map.insert<object_base_map::Mat4>(0x72f35d2, &m0); // mat4
				map.insert<object_base_map::Float>(0x2dd7ffe, 0.00f); // float
				map.insert<object_base_map::Mat4>(0xacaefb1, &local_t); // mat4
				map.insert<object_base_map::Int>(0x2c9331bd, 1); // int
				map.insert<object_base_map::Int>(0x26299a20, 1); // int
				map.insert<object_base_map::String>(0x355f55e0, R"(custom::anim_end)"); // string
				map.insert<object_base_map::Int>(0x65965327, 0); // int
				map.insert<object_base_map::Int>(0x52702583, 0); // int
				map.insert<object_base_map::Int>(0x525a07d4, 0); // int
				map.insert<object_base_map::String>(0x5b982501, R"(models\building_blocks\general\safehouse_guer_garage_door_col.pfx)"); // string
				map.insert<object_base_map::String>(0xca2ea3a9, R"(custom::opendoor)"); // string
				map.insert<object_base_map::String>(0xae7c2d5e, R"(custom::closedoor)"); // string
				map.insert<object_base_map::Int>(0x8597f162, 0); // int
				map.insert<object_base_map::Int>(0x78b67171, 1); // int
				map.insert<object_base_map::Int>(0xa1f2b8b1, 0); // int
				map.insert<object_base_map::String>(0xb8fbd88e, R"(CAnimatedRigidObject1)"); // string
				map.insert<object_base_map::Float>(0xb33b958d, 0.00f); // float
				map.insert<object_base_map::Float>(0xc868ac91, 0.00f); // float
				map.insert<object_base_map::Int>(0xe7916975, 0); // int
				map.insert<object_base_map::Int>(0xda3feaea, 1); // int
				map.insert<object_base_map::String>(0xef911d14, R"(CAnimatedRigidObject)"); // string
				map.insert<object_base_map::String>(0xea402acf, R"(building_blocks\general\safehouse_guer_garage_door.lod)"); // string
				map.insert<object_base_map::String>(0xf83e4d54, R"(building_blocks\animations\z_90.anim)"); // string

				test->init_from_map(&map);
				//test->get_event_manager()->setup_event_and_subscribe(0x284, "custom::opendoor");
				//test->get_event_manager()->setup_event_and_subscribe(0x288, "custom::closedoor");
			}

			{
				if (auto close_sound = g_game_control->create_object<SoundGameObject>())
				{
					object_base_map map{};

					map.insert<object_base_map::Float>(0x51f09be0, 0.00f); // float
					map.insert<object_base_map::Int>(0x402e727a, 1); // int
					map.insert<object_base_map::Mat4>(0xacaefb1, &local_t); // mat4
					map.insert<object_base_map::String>(0x425d6659, R"(door\door_garage.sab)"); // string
					map.insert<object_base_map::String>(0xb22eaffd, R"(custom::anim_end)"); // string
					map.insert<object_base_map::Float>(0x695d2cf2, 50.00f); // float
					map.insert<object_base_map::Int>(0x525a07d4, 0); // int
					map.insert<object_base_map::Float>(0x8dfff466, 0.00f); // float
					map.insert<object_base_map::Int>(0xbd63e5b2, -1); // int
					map.insert<object_base_map::String>(0xb8fbd88e, R"(snd_door_closed)"); // string
					map.insert<object_base_map::String>(0xe9b2e8b7, R"(custom::closedoor)"); // string
					map.insert<object_base_map::String>(0xe6d85c43, R"(custom::opendoor)"); // string
					map.insert<object_base_map::Int>(0xee2cc81d, 1); // int
					map.insert<object_base_map::String>(0xeb9f5c85, R"(door\door_garage.sob)"); // string
					map.insert<object_base_map::String>(0xef911d14, R"(CSoundGameObject)"); // string

					close_sound->init_from_map(&map);

					temp_vec2.push_back(std::move(close_sound));
				}

				if (auto open_sound = g_game_control->create_object<SoundGameObject>())
				{
					object_base_map map{};

					map.insert<object_base_map::Float>(0x51f09be0, 0.00f); // float
					map.insert<object_base_map::Int>(0x402e727a, 0); // int
					map.insert<object_base_map::Mat4>(0xacaefb1, &local_t); // mat4
					map.insert<object_base_map::String>(0x425d6659, R"(door\door_garage.sab)"); // string
					map.insert<object_base_map::String>(0xb22eaffd, R"(custom::opendoor)"); // string
					map.insert<object_base_map::Float>(0x695d2cf2, 100.f); // float
					map.insert<object_base_map::Int>(0x525a07d4, 0); // int
					map.insert<object_base_map::Float>(0x8dfff466, 0.00f); // float
					map.insert<object_base_map::Int>(0xbd63e5b2, -1); // int
					map.insert<object_base_map::String>(0xb8fbd88e, R"(snd_door_open)"); // string
					map.insert<object_base_map::Int>(0xee2cc81d, 1); // int
					map.insert<object_base_map::String>(0xeb9f5c85, R"(door\door_garage.sob)"); // string
					map.insert<object_base_map::String>(0xef911d14, R"(CSoundGameObject)"); // string

					jc::write<uint16_t>(jc::game::float_to_i16(500.f), *open_sound, 0xA4);
					jc::write<uint16_t>(jc::game::float_to_i16(500.f), *open_sound, 0xA6);

					open_sound->init_from_map(&map);

					jc::write<uint16_t>(jc::game::float_to_i16(500.f), *open_sound, 0xA4);
					jc::write<uint16_t>(jc::game::float_to_i16(500.f), *open_sound, 0xA6);

					log(RED, "open obj {:x}", ptr(*open_sound));

					temp_vec2.push_back(std::move(open_sound));
				}
			}

			log(RED, "nice {:x}", ptr(*test));

			g_game_control->enable_object(test);

			temp_vec.push_back(std::move(test));
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
		auto variant = NPCVariant::CREATE();

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