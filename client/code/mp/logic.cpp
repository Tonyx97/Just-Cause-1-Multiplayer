#include <defs/standard.h>

#include "logic.h"
#include "net.h"

#include <timer/timer.h>

#include <game/sys/world.h>
#include <game/object/character/character.h>

void jc::mp::logic::on_tick()
{
	// send and update our local player info

	static TimerRaw transform_timer(enet::TICKS_MS);
	static TimerRaw head_rotation_timer(enet::TICKS_MS);

	if (auto localplayer = g_net->get_localplayer())
		if (const auto local_char = localplayer->get_character())
		{
			const auto transform = local_char->get_transform();
			const auto skeleton = local_char->get_skeleton();
			const auto head_rotation = skeleton->get_head_euler_rotation();

			if (transform != localplayer->get_transform() && transform_timer.ready())
			{
				g_net->send_reliable(
					PlayerPID_DynamicInfo,
					PlayerDynInfo_Transform, transform);

				localplayer->set_transform(transform);
			}

			if (glm::distance2(head_rotation, localplayer->get_head_rotation()) > 7.5f && head_rotation_timer.ready())
			{
				g_net->send_reliable(PlayerPID_DynamicInfo, PlayerDynInfo_HeadRotation, head_rotation);

				localplayer->set_head_rotation(head_rotation);
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

			const auto player_char = player->get_character();

			check(player_char, "Player's character must be valid");

			const auto& move_info = player->get_movement_info();

			auto previous_transform = player_char->get_transform(),
				 target_transform = player->get_transform();

			// update the player movement

			player->skip_engine_stances = false;
			player_char->dispatch_movement(move_info.angle, move_info.right, move_info.forward, move_info.aiming);
			player->skip_engine_stances = player->is_alive();

			// interpolate the previous transform with the target one

			player_char->set_transform(previous_transform.interpolate(target_transform, 0.4f, 0.4f));

			break;
		}
		default: log(RED, "Unknown net object type {}", type);
		}
	});
}