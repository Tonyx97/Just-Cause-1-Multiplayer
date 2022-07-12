#include <defs/standard.h>

#include "tick.h"
#include "net.h"

#include <game/sys/world.h>
#include <game/object/character/character.h>

void jc::mp::on_tick()
{
	if (auto localplayer = g_net->get_localplayer())
	{
		g_net->send_reliable(PlayerPID_TickInfo, localplayer->get_tick_info());
	}
}