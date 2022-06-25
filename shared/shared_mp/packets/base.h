#pragma once

struct PacketBase
{
};

template <size_t size>
struct String
{
	char data[size] = { 0 };

	String() {}

	String& operator = (const std::string& str)
	{
		strcpy_s(data, str.c_str());

		return *this;
	}

	const char* c_str() const
	{
		return data;
	}

	std::string str() const
	{
		return c_str();
	}

	std::string operator * () const
	{
		return str();
	}
};