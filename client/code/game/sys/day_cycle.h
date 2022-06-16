#pragma once

namespace jc::day_cycle
{
	static constexpr uint32_t SINGLETON = 0xD33030; // DayCycle*

	static constexpr uint32_t HOURS_PER_DAY	  = 0x4;
	static constexpr uint32_t DAYS_PER_YEAR	  = 0x8;
	static constexpr uint32_t MONTHS_PER_YEAR = 0xC;
	static constexpr uint32_t YEAR			  = 0x10;
	static constexpr uint32_t DAY			  = 0x14;
	static constexpr uint32_t HOUR			  = 0x18;
	static constexpr uint32_t ENABLED		  = 0x1C;

	namespace g
	{
		static constexpr uint32_t NIGHT_TIME_OFF = 0xA1C840;
		static constexpr uint32_t NIGHT_TIME_ON  = 0xA1C838;
	}
}

class DayCycle
{
public:
	void init();
	void destroy();
	void set_time(int hour, int minute);
	void set_time(float v);
	void set_enabled(bool v);
	void reset_night_time();
	void set_night_time(float begin, float end);
	void set_night_time_enabled(bool v);

	bool is_enabled() const;
	bool is_night_time_enabled();

	int get_hours_per_day() const;
	int get_days_per_year() const;
	int get_months_per_year() const;
	int get_year() const;
	int get_day() const;

	float get_hour() const;
	float get_minute() const;
};

inline DayCycle* g_day_cycle = nullptr;