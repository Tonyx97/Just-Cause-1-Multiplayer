#include <defs/standard.h>

#include "time_system.h"

void TimeSystem::init()
{
}

void TimeSystem::destroy()
{
}

void TimeSystem::set_time_scale(float v)
{
	jc::write(v, this, jc::time_system::TIME_SCALE);
}

float TimeSystem::get_fps() const
{
	return jc::read<float>(this, jc::time_system::FPS);
}

float TimeSystem::get_delta() const
{
	return jc::read<float>(this, jc::time_system::DELTA);
}

float TimeSystem::get_last_fps() const
{
	return jc::read<float>(this, jc::time_system::LAST_FPS);
}

float TimeSystem::get_last_delta() const
{
	return jc::read<float>(this, jc::time_system::LAST_DELTA);
}

float TimeSystem::get_time_scale() const
{
	return jc::read<float>(this, jc::time_system::TIME_SCALE);
}