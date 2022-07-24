#pragma once

class Settings
{
private:

	float day_time = 0.f;

	bool day_time_enabled = false;

public:

	bool init();
	void destroy();

	void process();
	void set_day_time(float v) { day_time = v; }
	void set_day_time_enabled(bool v) { day_time_enabled = v; }

	float get_day_time() const { return day_time; }
};