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

uint64_t util::fs::get_last_write_time(const std::string& filename)
{
	std::wstring _filename = string::convert(filename);

#ifdef JC_CLIENT
	_filename = GET_MODULE_PATH() + _filename;
#endif

	return static_cast<uint64_t>(std::filesystem::last_write_time(_filename).time_since_epoch().count());
}

uint64_t util::fs::file_size(const std::string& filename)
{
	std::wstring _filename = string::convert(filename);

#ifdef JC_CLIENT
	_filename = GET_MODULE_PATH() + _filename;
#endif

	std::error_code ec;

	return std::filesystem::file_size(_filename, ec);
}

void util::fs::set_last_write_time(const std::string& filename, uint64_t new_time)
{
	std::wstring _filename = string::convert(filename);

#ifdef JC_CLIENT
	_filename = GET_MODULE_PATH() + _filename;
#endif

	const auto duration = std::chrono::file_clock::duration(new_time);
	const auto time_point = std::chrono::file_clock::time_point(duration);

	std::filesystem::last_write_time(_filename, time_point);
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

std::vector<char> util::fs::read_plain_file(const std::string& filename, bool zero_terminated)
{
#ifdef JC_CLIENT
	std::ifstream file(GET_MODULE_PATH() + string::convert(filename), std::ios::binary);
#else
	std::ifstream file(filename);
#endif

	if (!file)
		return {};

	const auto file_size = static_cast<size_t>(get_file_size(file));

	if (file_size <= 0u)
		return {};

	std::vector<char> data(file_size);

	file.read(data.data(), file_size);

	if (zero_terminated)
		data.push_back('\0');

	return data;
}

void util::fs::remove(const std::string& path)
{
	std::wstring _path = string::convert(path);

#ifdef JC_CLIENT
	_path = GET_MODULE_PATH() + _path;
#endif

	std::filesystem::remove(_path);
}

void util::fs::create_directory(const std::string& path)
{
	std::wstring _path = string::convert(path);

#ifdef JC_CLIENT
	_path = GET_MODULE_PATH() + _path;
#endif

	if (!std::filesystem::is_directory(_path))
		std::filesystem::create_directory(_path);
}

void util::fs::create_directories(const std::string& path)
{
	std::wstring _path = string::convert(path);

#ifdef JC_CLIENT
	_path = GET_MODULE_PATH() + _path;
#endif

	std::filesystem::create_directories(_path);
}

void util::fs::remove_empty_directories_in_directory(const std::string& path)
{
	std::wstring _path = string::convert(path);

#ifdef JC_CLIENT
	_path = GET_MODULE_PATH() + _path;
#endif

	std::vector<std::filesystem::path> empty_dirs;

	if (std::filesystem::is_directory(_path))
		for (const auto& d : std::filesystem::recursive_directory_iterator(_path))
			if (std::filesystem::is_directory(d) && std::filesystem::is_empty(d))
				empty_dirs.push_back(d);

	for (const auto& d : empty_dirs)
		std::filesystem::remove(d);
}

void util::fs::for_each_file_in_directory(const std::string& path, const iterate_fn_t& fn)
{
	std::wstring _path = string::convert(path);

#ifdef JC_CLIENT
	_path = GET_MODULE_PATH() + _path;
#endif

	for (const auto& d : std::filesystem::recursive_directory_iterator(_path))
		if (std::filesystem::is_regular_file(d))
			fn(d);
}

bool util::fs::is_empty(const std::string& path)
{
	std::wstring _path = string::convert(path);

#ifdef JC_CLIENT
	_path = GET_MODULE_PATH() + _path;
#endif

	return std::filesystem::is_empty(_path);
}

bool util::fs::is_directory(const std::string& path)
{
	std::wstring _path = string::convert(path);

#ifdef JC_CLIENT
	_path = GET_MODULE_PATH() + _path;
#endif

	return std::filesystem::is_directory(_path);
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