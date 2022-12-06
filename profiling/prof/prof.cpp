#include <Windows.h>
#include <TlHelp32.h>
#include <DbgHelp.h>
#include <fstream>

#include "prof.h"

#include <utils/utils.h>

#ifdef JC_CLIENT
#include <bug_ripper/bug_ripper.h>
#endif

namespace jc::prof
{
    HANDLE console_handle = nullptr;
    HWND console_hwnd = nullptr;
    bool console_allocated = false;

	uint32_t main_thread = 0u;

	int screen_width = 0,
		screen_height = 0;

    void init(const char* name)
    {
#if defined(JC_CLIENT) && !defined(JC_DBG)
		return;
#endif
        if (console_allocated)
			return;

        open_console(name);
    }

#ifdef JC_CLIENT
	void adjust_console()
	{
#if defined(JC_CLIENT) && !defined(JC_DBG)
		return;
#endif
		HWND game_window = nullptr;

		while (!game_window)
			game_window = FindWindow(L"Just Cause", nullptr);

		RECT game_window_rect;
		GetWindowRect(game_window, &game_window_rect);

		auto width = game_window_rect.right - game_window_rect.left;
		auto height = game_window_rect.bottom - game_window_rect.top;

		SetWindowPos(game_window, 0, (screen_width - width) / 2 + 250, (screen_height - height) / 2, width, height, 0);

		BringWindowToTop(game_window);
		SetForegroundWindow(game_window);
	}
#endif

    void open_console(const char* name)
	{
		if (!console_allocated)
		{
			FILE* temp;

			AllocConsole();

			freopen_s(&temp, "CONOUT$", "w", stdout);
			freopen_s(&temp, "CONIN$", "r", stdin);

			console_handle = CONSOLE_OUT;
			console_hwnd = GetConsoleWindow();

			console_allocated = true;

			screen_width = GetSystemMetrics(SM_CXSCREEN);
			screen_height = GetSystemMetrics(SM_CYSCREEN);

			SetConsoleTitleA(name);

			CONSOLE_FONT_INFOEX cfi{ 0 };
			cfi.cbSize = sizeof(cfi);

			GetCurrentConsoleFontEx(CONSOLE_OUT, FALSE, &cfi);

			cfi.dwFontSize.X = 0;
			cfi.dwFontSize.Y = 16;
			wcscpy_s(cfi.FaceName, L"Courier New");

			SetCurrentConsoleFontEx(CONSOLE_OUT, FALSE, &cfi);
			SetConsoleTextAttribute(CONSOLE_OUT, WHITE);

#ifdef JC_CLIENT
			SetWindowPos(console_hwnd, 0, 0, 0, 1000, 650, 0);
#endif

#ifdef _DEBUG
#ifdef JC_SERVER
			SetWindowPos(console_hwnd, 0, 0, 675, 1000, 650, 0);
#elif defined(JC_MASTERSV)
			SetWindowPos(console_hwnd, 0, -1000, 675, 1000, 650, 0);
#endif
#endif
		}
    }

    void close_console(bool free_console)
    {
#if defined(JC_CLIENT) && !defined(JC_DBG)
		return;
#endif

        if (!console_allocated)
            return;

#ifdef _DEBUG
#ifdef JC_CLIENT
		auto vs_window = FindWindow(nullptr, L"JustCauseMP - Microsoft Visual Studio");

		if (!vs_window)
			vs_window = FindWindow(nullptr, L"JustCauseMP (Running) - Microsoft Visual Studio");

		// bring visual studio back to top so we don't have to alt tab ourselves

		BringWindowToTop(vs_window);
		SetForegroundWindow(vs_window);
#endif
#endif

        ShowWindow(console_hwnd, SW_HIDE);

        if (free_console)
            FreeConsole();

        console_allocated = false;
    }

    namespace detail
    {
		void save_to_file(const std::string& text)
		{
			if (auto log_file = std::ofstream("logs.txt", std::ios::app))
				log_file << "[" + util::time::get_str_time() + "] " << text << '\n';
		}
		
        void log(bool nl, eColor color, const std::string& text)
        {
			save_to_file(text);

#if !defined(JC_CLIENT) || defined(JC_DBG)
			SetConsoleTextAttribute(console_handle, color);

			if (nl) printf_s("%s\n", text.c_str());
			else	printf_s("%s", text.c_str());
#endif
        }

        void log(bool nl, eColor color, const std::wstring& text)
        {
			save_to_file(util::string::convert(text));

#if !defined(JC_CLIENT) || defined(JC_DBG)
			SetConsoleTextAttribute(console_handle, color);

            if (nl) printf_s("%S\n", text.c_str());
            else	printf_s("%S", text.c_str());
#endif
		}

		void log(bool nl, bool show_time, eColor color, const std::string& text)
		{
			save_to_file(text);

#if !defined(JC_CLIENT) || defined(JC_DBG)
			SetConsoleTextAttribute(console_handle, color);

			auto time_str = util::time::get_str_time();

			if (show_time)
			{
				if (nl)
					printf_s("[%s] %s\n", time_str.c_str(), text.c_str());
				else
					printf_s("[%s] %s", time_str.c_str(), text.c_str());
			}
			else
			{
				if (nl)
					printf_s("%s\n", text.c_str());
				else
					printf_s("%s", text.c_str());
			}
#endif
		}
	}

	bool error_internal(const std::string& text, int type)
	{
#ifdef JC_CLIENT
		jc::bug_ripper::about_to_throw_error = true;
#endif

		if (type == 0)
		{
			MessageBoxA(nullptr, text.c_str(), "JC:MP", MB_OK | MB_ICONERROR);

#ifdef JC_DBG
			__debugbreak();
#endif

			TerminateProcess(GetCurrentProcess(), 1);
		}
		else
		{
			MessageBoxA(nullptr, text.c_str(), "JC:MP", MB_OK | MB_ICONWARNING);

#ifdef JC_DBG
			__debugbreak();
#endif

			return (type == 2);
		}

#ifdef JC_DBG
		__debugbreak();
#endif

		return false;
	}

	void set_main_thread()
	{
		main_thread = GetCurrentThreadId();
	}

	void check_main_thread()
	{
		check(GetCurrentThreadId() == main_thread, "This code cannot be executed from thread {:x} (main thread {:x})", GetCurrentThreadId(), main_thread);
	}
}