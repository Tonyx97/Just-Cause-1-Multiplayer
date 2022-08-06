#include <defs/standard.h>

#include "utils.h"

namespace util::time
{
	std::chrono::high_resolution_clock::time_point process_startup_tp;
	
	tm get_tm_date()
	{
		auto tick = ::time(0);
		tm tm;
		localtime_s(&tm, &tick);
		return tm;
	}

	std::string get_str_date()
	{
		const auto time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());

		return std::format("{:%Y-%m-%d}", time);
	}

	std::string get_str_time()
	{
		auto date = get_tm_date();

		auto hour_prefix = (date.tm_hour < 10 ? "0" : "") + std::to_string(date.tm_hour),
				minute_prefix = (date.tm_min < 10 ? "0" : "") + std::to_string(date.tm_min),
				second_prefix = (date.tm_sec < 10 ? "0" : "") + std::to_string(date.tm_sec);

		return hour_prefix + ':' + minute_prefix + ':' + second_prefix;
	}

	std::string get_str_time_path()
	{
		auto date = get_tm_date();

		auto hour_prefix = (date.tm_hour < 10 ? "0" : "") + std::to_string(date.tm_hour),
			minute_prefix = (date.tm_min < 10 ? "0" : "") + std::to_string(date.tm_min),
			second_prefix = (date.tm_sec < 10 ? "0" : "") + std::to_string(date.tm_sec);

		return hour_prefix + '.' + minute_prefix + '.' + second_prefix;
	}

	float get_time()
	{
		return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - process_startup_tp).count());
	}
}

void util::init()
{
	rand::init_seed();

	time::process_startup_tp = std::chrono::high_resolution_clock::now();
}

void util::win::get_desktop_resolution(int32_t& x, int32_t& y)
{
	MONITORINFO info = {};

	info.cbSize = sizeof(MONITORINFO);

	GetMonitorInfo(MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST), &info);

	x = info.rcMonitor.right - info.rcMonitor.left;
	y = info.rcMonitor.bottom - info.rcMonitor.top;
}