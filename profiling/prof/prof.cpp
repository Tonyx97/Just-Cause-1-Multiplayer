#include <Windows.h>

#include "prof.h"

#include <utils/utils.h>

namespace jc::prof
{
    HANDLE console_handle = nullptr;
    HWND console_hwnd = nullptr;
    bool console_allocated = false;

    void init(const char* name)
    {
        if (console_allocated)
			return;

        open_console(name);
    }

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

#ifdef JC_CLIENT
			auto screen_width = GetSystemMetrics(SM_CXSCREEN);
			auto screen_height = GetSystemMetrics(SM_CYSCREEN);

			SetConsoleTitleA(name);
			SetWindowPos(console_hwnd, 0, 0, 0, 1000, 650, 0);

			CONSOLE_FONT_INFOEX cfi { 0 };
			cfi.cbSize = sizeof(cfi);

			GetCurrentConsoleFontEx(CONSOLE_OUT, FALSE, &cfi);

			cfi.dwFontSize.X = 0;
			cfi.dwFontSize.Y = 16;
			wcscpy_s(cfi.FaceName, L"Courier New");

			SetCurrentConsoleFontEx(CONSOLE_OUT, FALSE, &cfi);
			SetConsoleTextAttribute(CONSOLE_OUT, WHITE);

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
#endif

#ifdef JC_SERVER
#ifdef _DEBUG
			SetWindowPos(console_hwnd, 0, 0, 675, 1000, 650, 0);
#endif
#endif
		}
    }

    void close_console(bool free_console)
    {
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
        void log(bool nl, eColor color, const std::string& text)
        {
            SetConsoleTextAttribute(console_handle, color);

            if (nl) printf_s("%s\n", text.c_str());
            else	printf_s("%s", text.c_str());
        }

        void log(bool nl, eColor color, const std::wstring& text)
        {
            SetConsoleTextAttribute(console_handle, color);

            if (nl) printf_s("%S\n", text.c_str());
            else	printf_s("%S", text.c_str());
		}

		void log(bool nl, bool show_time, eColor color, const std::string& text)
		{
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
		}

        void log_info(bool nl, const char* filename, int line, eColor color, const char* text)
        {
            std::string file_str(filename);

            auto real_pos = file_str.substr(0, file_str.find_last_of('\\') - 1).find_last_of('\\') + 1;
            auto fixed_filename = file_str.substr(real_pos, file_str.length() - real_pos);

            SetConsoleTextAttribute(console_handle, WHITE);		printf_s("[Line ");
            SetConsoleTextAttribute(console_handle, CYAN);		printf_s("%i", line);
            SetConsoleTextAttribute(console_handle, WHITE);		printf_s(": ");
            SetConsoleTextAttribute(console_handle, PURPLE);	printf_s("%s", fixed_filename.c_str());
            SetConsoleTextAttribute(console_handle, WHITE);		printf_s("] ");
            SetConsoleTextAttribute(console_handle, color);

            log(nl, color, text);
        }
	}

	bool error_internal(const std::string& text, int type)
	{
		if (type == 0)
		{
			MessageBoxA(nullptr, text.c_str(), "JC:MP", MB_OK | MB_ICONERROR);

			__debugbreak();

			TerminateProcess(GetCurrentProcess(), 1);
		}
		else
		{
			MessageBoxA(nullptr, text.c_str(), "JC:MP", MB_OK | MB_ICONWARNING);

			__debugbreak();

			return (type == 2);
		}

		__debugbreak();

		return false;
	}
}