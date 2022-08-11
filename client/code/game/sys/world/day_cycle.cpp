#include <defs/standard.h>

#include "day_cycle.h"

void DayCycle::init()
{
	set_enabled(false);
}

void DayCycle::destroy()
{
	set_enabled(true);
}

void DayCycle::set_time(int hour, int minute)
{
	if (hour >= 0.f && hour <= get_hours_per_day() && minute >= 0.f && minute <= 60.f)
		jc::write(static_cast<float>(hour) + (static_cast<float>(minute) / 60.f), this, jc::day_cycle::HOUR);
}

void DayCycle::set_time(float v)
{
	if (v >= 0.f && v <= get_hours_per_day())
		jc::write(v, this, jc::day_cycle::HOUR);
}

void DayCycle::set_enabled(bool v)
{
	jc::write(v, this, jc::day_cycle::ENABLED);
}

void DayCycle::reset_night_time()
{
	jc::write(18.5, jc::day_cycle::g::NIGHT_TIME_ON);
	jc::write(5.5, jc::day_cycle::g::NIGHT_TIME_OFF);
}

void DayCycle::set_night_time(float begin, float end)
{
	jc::write(static_cast<double>(begin), jc::day_cycle::g::NIGHT_TIME_ON);
	jc::write(static_cast<double>(end), jc::day_cycle::g::NIGHT_TIME_OFF);
}

void DayCycle::set_night_time_enabled(bool v)
{
	if (v)
	{
		jc::write(std::numeric_limits<double>::lowest(), jc::day_cycle::g::NIGHT_TIME_ON);
		jc::write(std::numeric_limits<double>::max(), jc::day_cycle::g::NIGHT_TIME_OFF);
	}
	else
	{
		jc::write(std::numeric_limits<double>::max(), jc::day_cycle::g::NIGHT_TIME_ON);
		jc::write(std::numeric_limits<double>::lowest(), jc::day_cycle::g::NIGHT_TIME_OFF);
	}
}

bool DayCycle::is_enabled() const
{
	return jc::read<bool>(this, jc::day_cycle::ENABLED);
}

bool DayCycle::is_night_time_enabled()
{
	return jc::read<double>(jc::day_cycle::g::NIGHT_TIME_ON) < -1000.0 &&
		   jc::read<double>(jc::day_cycle::g::NIGHT_TIME_OFF) > 1000.0;
}

int DayCycle::get_hours_per_day() const
{
	return jc::read<int>(this, jc::day_cycle::HOURS_PER_DAY);
}

int DayCycle::get_days_per_year() const
{
	return jc::read<int>(this, jc::day_cycle::DAYS_PER_YEAR);
}

int DayCycle::get_months_per_year() const
{
	return jc::read<int>(this, jc::day_cycle::MONTHS_PER_YEAR);
}

int DayCycle::get_year() const
{
	return jc::read<int>(this, jc::day_cycle::YEAR);
}

int DayCycle::get_day() const
{
	return jc::read<int>(this, jc::day_cycle::DAY);
}

float DayCycle::get_hour() const
{
	return jc::read<float>(this, jc::day_cycle::HOUR);
}

float DayCycle::get_minute() const
{
	const auto hour = get_hour();

	return hour - std::floorf(hour);
}