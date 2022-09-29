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

bool util::win::set_current_directory(const std::wstring& new_dir)
{
	SetCurrentDirectoryW(new_dir.c_str());

	return GetLastError() == ERROR_SUCCESS;
}

std::wstring util::win::get_current_directory()
{
	wchar_t dir[MAX_PATH] = { 0 };

	GetCurrentDirectoryW(MAX_PATH, dir);

	if (GetLastError() != ERROR_SUCCESS)
		return {};

	return std::wstring(dir);
}

std::tuple<void*, size_t> util::win::load_resource(void* mod_base, int id, LPWSTR type)
{
	const auto mod_module = (HMODULE)mod_base;
	const auto resource = FindResource(mod_module, MAKEINTRESOURCE(id), type);
	const auto rsrc_mem = LoadResource(mod_module, resource);
	const auto size = SizeofResource(mod_module, resource);
	const auto rsrc_address = LockResource(rsrc_mem);
	const auto font_mem = malloc(size);

	memcpy(font_mem, rsrc_address, size);

	UnlockResource(resource);

	return { font_mem, size };
}

int64_t util::fs::get_file_size(std::ifstream& file)
{
	file.seekg(0, std::ios::end);

	const auto length = file.tellg();

	file.seekg(0);

	return static_cast<int64_t>(length);
}

std::string util::fs::strip_parent_path(const std::string& str)
{
	const auto pos = str.find_last_of('\\');
	
	if (pos == std::string::npos)
		return str;
	
	return str.substr(pos + 1);
}

std::vector<uint8_t> util::fs::read_bin_file(const std::string& filename)
{
#ifdef JC_CLIENT
	std::ifstream file(GET_MODULE_PATH() + string::convert(filename), std::ios::binary);
#else
	std::ifstream file(filename, std::ios::binary);
#endif

	if (!file)
		return {};

	const auto file_size = static_cast<size_t>(get_file_size(file));

	if (file_size <= 0u)
		return {};

	std::vector<uint8_t> data(file_size);

	file.read(BITCAST(char*, data.data()), file_size);

	return data;
}

bool util::fs::create_bin_file(const std::string& filename, const std::vector<uint8_t>& data)
{
#ifdef JC_CLIENT
	std::ofstream file(GET_MODULE_PATH() + string::convert(filename), std::ios::binary);
#else
	std::ofstream file(filename, std::ios::binary);
#endif

	if (!file)
		return false;

	file.write(BITCAST(char*, data.data()), data.size());

	return true;
}