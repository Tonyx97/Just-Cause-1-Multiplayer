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
	void update();
	void set_time_scale(float v);
	void set_day_time(float v);
	void set_day_time_enabled(bool v);
	void set_punch_force(float v);

	bool is_day_time_enabled() const { return day_time_enabled; }

	float get_time_scale() const { return timescale; }
	float get_day_time() const { return day_time; }
	float get_punch_force() const { return punch_force; }
};