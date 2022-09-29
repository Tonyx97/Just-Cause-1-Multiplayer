#include <defs/standard.h>

#include <timer/timer.h>

#include "settings.h"

#include "net.h"

bool Settings::init()
{
	return true;
}

void Settings::destroy()
{
}

void Settings::update()
{
	static TimerRaw day_cycle_timer(1000);

	// send the day time to all players

	if (day_cycle_timer.ready())
	{
		day_time += 0.01f;

		g_net->send_broadcast(Packet(WorldPID_SetTime, ChannelID_World, day_time_enabled, day_time));
	}
}