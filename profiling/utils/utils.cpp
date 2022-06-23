#include <defs/standard.h>

#include "utils.h"

namespace util::time
{
	std::string get_str_date()
	{
		const auto time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());

		return std::format("{:%Y-%m-%d}", time);
	}

	std::string get_str_time()
	{
		const auto time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());

		return std::format("{:%X}", time);
	}
}