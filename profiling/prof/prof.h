#pragma once

#include <string>
#include <format>
#include <source_location>
#include <chrono>

#define CONSOLE_OUT		GetStdHandle(STD_OUTPUT_HANDLE)
#define CONSOLE_IN		GetStdHandle(STD_INPUT_HANDLE)

enum eColor : unsigned short
{
	BLACK = 0x0,
	DARK_BLUE = 0x1,
	DARK_GREEN = 0x2,
	DARK_CYAN = 0x3,
	DARK_RED = 0x4,
	DARK_PURPLE = 0x5,
	DARK_YELLOW = 0x6,
	GREY = 0x7,
	DARK_GREY = 0x8,
	BLUE = 0x9,
	GREEN = 0xA,
	CYAN = 0xB,
	RED = 0xC,
	PURPLE = 0xD,
	YELLOW = 0xE,
	WHITE = 0xF,
};

#define FORMAT(t, a) std::vformat(t, std::make_format_args(a...))
#define FORMATV(t, ...) std::vformat(t, std::make_format_args(__VA_ARGS__))

namespace jc::prof
{
    void init(const char* name);
    void open_console(const char* name);
    void close_console(bool free_console);

    namespace detail
    {
        void log(bool nl, eColor color, const std::string& text);
		void log(bool nl, eColor color, const std::wstring& text);
		void log(bool nl, bool show_time, eColor color, const std::string& text);
        void log_info(bool nl, const char* filename, int line, eColor color, const char* text);
    }

    template <typename T, typename... A>
    inline void log(bool nl, eColor color, const T text, A... args)
    {
        detail::log(nl, false, color, FORMAT(text, args));
    }

    template <typename T, typename... A>
    inline void log(eColor color, const T text, A... args)
    {
		detail::log(true, color, FORMAT(text, args));
	}

	template <typename T, typename... A>
	inline void logt(eColor color, const T text, A... args)
	{
		detail::log(true, true, color, FORMAT(text, args));
	}

	template <typename T, typename... A>
	inline bool logb(eColor color, const T text, A... args)
	{
		detail::log(true, color, FORMAT(text, args));
		return false;
	}

	template <typename T, typename... A>
	inline bool logbw(eColor color, const T text, A... args)
	{
		detail::log(true, color, FORMAT(text, args));
		Sleep(500);
		return false;
	}

	template <typename T, typename... A>
	inline bool logbt(eColor color, const T text, A... args)
	{
		detail::log(true, true, color, FORMAT(text, args));
		return false;
	}

	template <typename T, typename... A>
	inline bool logbwt(eColor color, const T text, A... args)
	{
		detail::log(true, true, color, FORMAT(text, args));
		Sleep(500);
		return false;
	}

    template <typename T, typename... A>
    inline void log_nl(eColor color, const T text, A... args)
    {
        detail::log(false, color, FORMAT(text, args));
    }

	bool error_internal(const std::string& text, int type);
	
	template <typename... A>
	inline void check(bool condition, const char* text, A... args)
	{
		if (condition)
			return;

		error_internal(FORMAT(text, args), 0);
	}

	struct TimeProfiling
	{
		std::chrono::high_resolution_clock::time_point m_start;
		uint64_t									   cycles	 = 0;
		char										   name[128] = { 0 };

		TimeProfiling(const char* name)
		{
			strcpy_s(this->name, name);
			m_start = std::chrono::high_resolution_clock::now();
			cycles	= __rdtsc();
		}

		~TimeProfiling()
		{
			const auto cycles_passed = __rdtsc() - cycles;
			const auto time_passed	 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_start).count();
			log(true, YELLOW, "%s: %.3f ms | %i mcs | %i cycles", name, static_cast<double>(time_passed) / 1000.f, time_passed, cycles_passed);
		}
	};
}

using namespace jc::prof;

#define CURR_FN std::source_location::current().function_name()