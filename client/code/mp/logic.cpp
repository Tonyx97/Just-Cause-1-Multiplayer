#include <defs/standard.h>

#include "logic.h"
#include "net.h"

#include <timer/timer.h>

#include <game/sys/world.h>
#include <game/object/character/character.h>

void jc::mp::logic::on_tick()
{
	struct LocalInfo
	{
		Player* player = nullptr;

		Character* character = nullptr;

		operator bool() const { return player && character; }
	};

	auto get_local_instances = []() -> LocalInfo
	{
		if (auto localplayer = g_net->get_localplayer())
			if (const auto local_char = g_world->get_localplayer_character())
				return { localplayer, local_char };

		return { nullptr, nullptr };
	};

	static auto transform_sync = timer::add_timer(10, [&get_local_instances]()
	{
		if (const auto info = get_local_instances())
		{
			g_net->send_reliable(PlayerPID_DynamicInfo, 0u, info.character->get_transform());
			g_net->send_reliable(PlayerPID_DynamicInfo, 1u, info.character->get_velocity());
		}
	});

	static auto head_rotation_sync = timer::add_timer(30, [&get_local_instances]()
	{
		if (const auto info = get_local_instances())
			g_net->send_reliable(PlayerPID_DynamicInfo, 2u, info.character->get_skeleton()->get_head_euler_rotation());
	});

	timer::dispatch();
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

			//if (glm::length(player_char->get_velocity()) < 0.1f && GetAsyncKeyState(VK_F1))
			{
				player_char->set_transform(previous_transform.interpolate(target_transform, 0.5f, 0.5f));
			}

			break;
		}
		default: log(RED, "Unknown net object type {}", type);
		}
	});
}