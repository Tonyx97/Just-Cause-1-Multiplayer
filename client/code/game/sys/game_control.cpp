#include <defs/standard.h>

#include "game_control.h"

#include <timer/timer.h>

#include <mp/net.h>

#include <game/object/character/character.h>
#include <game/object/localplayer/localplayer.h>
#include <game/object/character_handle/character_handle.h>
#include <game/sys/world.h>
#include <game/sys/factory_system.h>
#include <core/keycode.h>
#include <core/test_units.h>

DEFINE_HOOK_THISCALL_S(tick, 0x4036F0, bool, void* _this)
{
	jc::hooks::HookLock lock {};

	const auto res = tick_hook.call(_this);

	{
		if (auto local_char = g_world->get_localplayer_character())
		{
			auto localplayer = g_world->get_localplayer();

			auto local_transform = local_char->get_transform();

			static CharacterHandle* cc_h = nullptr;

			if (g_key->is_key_pressed(VK_F4))
				localplayer->get_character()->set_hp(1.f);

			if (g_key->is_key_pressed(VK_F5))
				localplayer->respawn();

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
					g_factory->destroy_character_handle(cc_h);
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

	timer::dispatch();

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
	tick_hook.hook();
}

void GameControl::unhook_tick()
{
	tick_hook.unhook();
}