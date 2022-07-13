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
			localplayer->set_transform(local_char->get_transform());

			g_net->send_reliable(PlayerPID_TickInfo, localplayer->get_tick_info());
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

			//player->set_body_stance_id(player->get_body_stance_id());
			//player->set_arms_stance_id(player->get_arms_stance_id());

			//log(RED, "updating player {:x}", player->get_nid());

			break;
		}
		default: log(RED, "Unknown net object type {}", type);
		}
	});
}