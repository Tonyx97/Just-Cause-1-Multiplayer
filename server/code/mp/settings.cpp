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
		set_day_time(get_day_time() + 0.001f);
}

void Settings::set_time_scale(float v)
{
	timescale = v;

	g_net->send_broadcast(Packet(WorldPID_SetTimeScale, ChannelID_World, v));
}

void Settings::set_day_time(float v)
{
	day_time = v;

	g_net->send_broadcast(Packet(WorldPID_SetTime, ChannelID_World, day_time_enabled, v));
}

void Settings::set_day_time_enabled(bool v)
{
	day_time_enabled = v;

	set_day_time(get_day_time());
}

void Settings::set_punch_force(float v)
{
	punch_force = v;

	g_net->send_broadcast(Packet(WorldPID_SetPunchForce, ChannelID_World, v));
}

void Settings::set_gravity(const vec3& v)
{
	gravity = v;

	g_net->send_broadcast(Packet(WorldPID_SetGravity, ChannelID_World, v));
}