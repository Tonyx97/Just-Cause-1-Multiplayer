#include <defs/standard.h>

#include "logic.h"
#include "net.h"

#include <game/sys/world.h>
#include <game/object/character/character.h>

void jc::mp::logic::on_tick()
{
	if (auto localplayer = g_net->get_localplayer())
	{
		if (const auto local_char = g_world->get_localplayer_character())
		{
			const auto skeleton = local_char->get_skeleton();
			const auto transform = local_char->get_transform();

			localplayer->set_transform(transform);

			//g_net->send_reliable(PlayerPID_TickInfo, localplayer->get_tick_info());
			g_net->send_reliable(PlayerPID_DynamicInfo, 0u, transform);
			g_net->send_reliable(PlayerPID_DynamicInfo, 1u, skeleton->get_head_euler_rotation());
		}
	}
}

void jc::mp::logic::on_update_objects()
{
	g_net->for_each_net_object([](NID, NetObject* obj)
	{
		switch (const auto type = obj->get_type())
		{
		case NetObject_Player:
		{
			const auto player = obj->cast<Player>();

			if (player->is_local())
				break;

			player->skip_engine_stances = false;
			jc::hooks::call<jc::character::hook::dispatch_movement_t>(player->get_character(), player->angle, player->right, player->forward, player->aiming);
			player->skip_engine_stances = player->is_alive();

			//player->set_body_stance_id(player->get_body_stance_id());
			//player->set_arms_stance_id(player->get_arms_stance_id());

			break;
		}
		default: log(RED, "Unknown net object type {}", type);
		}
	});
}