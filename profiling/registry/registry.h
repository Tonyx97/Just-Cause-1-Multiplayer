#pragma once

class Registry
{
private:

	HKEY key = nullptr;

public:

	bool init();

	void destroy();

	std::string get_string(const char* value_name);

	int get_int(const char* value_name);
	
	bool set_string(const char* value_name, const std::string& value);
};

inline Registry g_registry;