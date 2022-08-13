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

void Settings::process()
{
	static TimerRaw day_cycle_timer(1000);
	static TimerRaw rand_seed_timer(5000);

	// send the day time to all players

	if (day_cycle_timer.ready())
	{
		day_time += 0.01f;

		g_net->send_broadcast_reliable<ChannelID_World>(WorldPID_SetTime, day_time_enabled, day_time);
	}

	// sync a random seed so the synchronization between clients is a bit
	// better, this is just a helper to visuals

	if (rand_seed_timer.ready())
		g_net->send_broadcast_reliable<ChannelID_World>(WorldPID_SetRandSeed, rand_seed = __rdtsc());
}