#include <defs/standard.h>

#include "game_control.h"

#include <mp/net.h>

#include <game/object/character/character.h>
#include <game/object/character_handle/character_handle.h>
#include <game/sys/world.h>
#include <game/sys/factory_system.h>
#include <core/keycode.h>
#include <core/test_units.h>

bool __fastcall GameControl::hk_tick(void* _this)
{
	jc::hooks::HookLock lock {};

	const auto res = jc::hooks::call<jc::proto::game_tick>(_this);

	{
		if (auto local_char = g_world->get_localplayer_character())
		{
			auto localplayer = g_world->get_localplayer();

			auto local_transform = local_char->get_transform();

			static CharacterHandle* cc_h = nullptr;

			if (g_key->is_key_down(VK_F5))
			{
				auto lp = ptr(localplayer);
				auto lpc = ptr(local_char);

				for (int i = 0; i < 1000; ++i)
					local_char->respawn();

				/*jc::this_call(0x4D1480, localplayer, 0);
				jc::this_call(0x58DC90, local_char);
				// 
				jc::this_call(0x4C34C0, localplayer);
				*(bool*)(lp + 0x4AC) = 1;
				jc::write<bool>(true, 0xD85B8C);
				jc::std_call(0x58C960, 0);
				jc::this_call(0x59F8E0, lpc);

				jc::this_call(0x603760, lp + 0x24);

				jc::this_call(0x485290, jc::read<ptr>(0xD32EE4), 0);
				jc::this_call(0x488410, jc::read<ptr>(0xD32EE4), jc::read<ptr>(0xD32EE4) + 0x20);*/
			}

			if (g_key->is_key_pressed(VK_F6))
			{
				if (!g_test_char)
				{
					cc_h = g_factory->spawn_character("female1", g_world->get_localplayer_character()->get_position());
					g_test_char = cc_h->get_character();
					g_test_char->set_model(126);
					g_test_char->set_invincible(true);
				}
				else
				{
					//cc->respawn();
					cc_h->destroy();
					cc_h = nullptr;
					g_test_char = nullptr;
				}

				log(RED, "{:x}", ptr(g_test_char));
			}

			if (g_test_char && g_test_char->is_alive())
			{
				// interpolate main transform

				auto previous_t = g_test_char->get_transform();

				local_transform.translate(vec3(0.f, 0.f, 2.f));

				previous_t.interpolate(local_transform, 0.2f, 0.05f);

				g_test_char->set_transform(previous_t);

				vec3 vel = {};

				// interpolate head rotation

				const auto target = local_char->get_skeleton()->get_head_euler_rotation();

				if (glm::length(target) > 0.f)
				{
					g_test_char->get_skeleton()->set_head_euler_rotation(target);
				}

				/*if (g_key->is_key_down(VK_NEXT))
				{
					static int i = 0;

					//if (++i % 10 == 0)
						jc::this_call(0x5A45D0, g_test_char, 0.f, 0.f, 1.f, true);
				}*/
			}
		}
	}

	g_net->tick();
	g_net->update_objects();
	g_key->clear_states();

	return res;
}

void GameControl::init()
{
}

void GameControl::destroy()
{
}

void GameControl::hook_tick()
{
	jc::hooks::hook<jc::proto::game_tick>(&hk_tick);
}

void GameControl::unhook_tick()
{
	jc::hooks::unhook<jc::proto::game_tick>();
}