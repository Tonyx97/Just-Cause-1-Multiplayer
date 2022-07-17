#include <defs/standard.h>

#include "utils.h"

namespace util::time
{
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
}