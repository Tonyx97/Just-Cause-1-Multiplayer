#include <defs/standard.h>

#include "registry.h"

bool Registry::init()
{
	if (RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\JCMP\\Game", &key) != ERROR_SUCCESS)
		return logb(RED, "Could not create registry key");

	DWORD size = 0;
	char name[16] { 0 };

	if (RegQueryValueEx(key, L"Username", nullptr, nullptr, (BYTE*)name, &size) == ERROR_FILE_NOT_FOUND)
	{
		const std::string default_name = "Player_" + std::to_string(uint64_t(this));

		RegSetValueExA(key, "Username", 0, REG_SZ, (const BYTE*)default_name.data(), default_name.length());
	}

	return true;
}

void Registry::destroy()
{
	RegCloseKey(key);
}

std::string Registry::get_string(const char* value_name)
{
	DWORD size = MAX_PATH;
	char temp_data[MAX_PATH] { 0 };

	return (RegQueryValueExA(key, value_name, nullptr, nullptr, (BYTE*)temp_data, &size) == ERROR_SUCCESS ? std::string(temp_data) : "");
}

int Registry::get_int(const char* value_name)
{
	int value = 0;

	DWORD size = sizeof(value);

	DWORD type = REG_DWORD;

	return (RegQueryValueExA(key, value_name, nullptr, &type, (BYTE*)&value, &size) == ERROR_SUCCESS ? value : 0);
}

bool Registry::set_string(const char* value_name, const std::string& value)
{
	return (RegSetValueExA(key, value_name, 0, REG_SZ, (const BYTE*)value.data(), value.length()) == ERROR_SUCCESS);
}