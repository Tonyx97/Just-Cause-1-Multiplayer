#pragma once

class Settings
{
private:

	float timescale = 1.f,
		  day_time = 8.f,
		  punch_force = 50.f;

	bool day_time_enabled = false;

public:

	bool init();

	void destroy();

	void process();
	void set_time_scale(float v) { timescale = v; }
	void set_day_time(float v) { day_time = v; }
	void set_day_time_enabled(bool v) { day_time_enabled = v; }
	void set_punch_force(float v) { punch_force = v; }

	float get_time_scale() const { return timescale; }
	float get_day_time() const { return day_time; }
	float get_punch_force() const { return punch_force; }
};