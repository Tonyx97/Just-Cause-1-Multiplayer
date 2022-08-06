#include <defs/standard.h>

#include "bug_ripper.h"

#ifdef JC_CLIENT
#include <core/ui.h>
#endif

#define IDD_CRASH_DIALOG            114
#define IDC_SEND_DESC				9701
#define IDC_SEND_QUESTION			9702
#define IDC_INFO_EDIT				9703
#define IDC_CRASH_HEAD				9704

#pragma comment (lib, "dbghelp.lib")
#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

bool g_can_close_crash_wnd = false;

INT_PTR __stdcall crash_wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
long __stdcall global_veh(_EXCEPTION_POINTERS* ep);

namespace jc::bug_ripper
{
	struct Module
	{
		ptr base = 0u;
		size_t size = 0u;

		void* as_ptr() const { return std::bit_cast<void*>(base); }
		HMODULE as_module() const { return std::bit_cast<HMODULE>(base); }

		bool contains_address(ptr v) { return v >= base && v < base + size; }
	};

	Module game_mod {},
		   mod_mod {};

	bool init(void* mod_base)
	{
#ifdef _DEBUG
		return true;
#else
		if (AddVectoredExceptionHandler(1, global_veh))
		{
			game_mod.base = ptr(GetModuleHandle(nullptr));
			mod_mod.base = ptr(mod_base);

			util::mem::for_each_module(GetCurrentProcessId(), [&](uintptr_t base_addr, uint32_t size, const wchar_t*)
			{
				if (base_addr == game_mod.base)
					game_mod.size = size;
				else if (base_addr == mod_mod.base)
					mod_mod.size = size;

				return game_mod.size > 0 && mod_mod.size > 0;
			}, {});

			log(RED, "Game Info: {:x} {}", game_mod.base, game_mod.size);
			log(RED, "Module Info: {:x} {}", mod_mod.base, mod_mod.size);

			return true;
		}

		return false;
#endif
	}

	bool destroy()
	{
#ifdef JC_DBG
		return true;
#else
		return RemoveVectoredExceptionHandler(global_veh);
#endif
	}

	uintptr_t get_module_info_if_valid(uintptr_t& addr, wchar_t* module_name)
	{
		uintptr_t mod_base = 0;

		util::mem::for_each_module(GetCurrentProcessId(), [&addr, module_name, &mod_base](uintptr_t base_addr, uint32_t size, const wchar_t* name)
		{
			if (addr >= base_addr && addr < base_addr + size)
			{
				std::wcscpy(module_name, name);
				mod_base = base_addr;
				addr -= base_addr;
				return true;
			}

			return false;
		}, {});

		return mod_base;
	}

	long show_and_dump_crash(_EXCEPTION_POINTERS* ep)
	{
		auto eip = uintptr_t(ep->ExceptionRecord->ExceptionAddress);

		const bool exception_on_game = game_mod.contains_address(eip);
		const bool exception_on_mod = mod_mod.contains_address(eip);

		if (!exception_on_game && !exception_on_mod)
			return EXCEPTION_CONTINUE_SEARCH;

		wchar_t mod_name[256] { 0 };

		if (auto mod_base = get_module_info_if_valid(eip, mod_name))
		{
#ifdef JC_CLIENT
			ShowWindow(g_ui->get_window(), SW_HIDE);
			ShowCursor(true);
#endif

			auto dialog = CreateDialogW(mod_mod.as_module(), MAKEINTRESOURCEW(IDD_CRASH_DIALOG), nullptr, crash_wnd_proc);

#ifdef JC_CLIENT
			SendMessageW(dialog, WM_SETICON, ICON_SMALL, GET_GAME_ICON());
#endif

			RECT rect;

			GetWindowRect(dialog, &rect);

			const int width = rect.right - rect.left;
			const int height = rect.bottom - rect.top;

			SetWindowText(dialog, L"Just Cause Multiplayer (Bug Ripper)");
			SetWindowPos(dialog, HWND_TOPMOST, (GetSystemMetrics(SM_CXSCREEN) - width) / 2, (GetSystemMetrics(SM_CYSCREEN) - height) / 2, width, height, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			BringWindowToTop(dialog);

			std::wstring buffer = L"The game crashed, we need to gather information about this crash.\r\nPlease send us report so we can fix the issue as soon as possible.\r\nMore Information about this crash in game's logs.\r\n\r\n";
			
			if (!std::filesystem::is_directory("Crash Logs"))
				std::filesystem::create_directory("Crash Logs");

			std::wofstream log("Crash Logs\\CRASH DUMP " + util::time::get_str_date() + ' ' + util::time::get_str_time_path() + ".log", std::ios::trunc);

			log << "- Exception Code: " << std::hex << ep->ExceptionRecord->ExceptionCode << std::endl;
			log << "- GP Registers:" << std::endl;
			log << "\tEip: " << mod_name << " + 0x" << std::hex << eip << std::endl;
			log << "\tEbp: 0x" << std::hex << ep->ContextRecord->Ebp << std::endl;
			log << "\tEax: 0x" << std::hex << ep->ContextRecord->Eax << std::endl;
			log << "\tEbx: 0x" << std::hex << ep->ContextRecord->Ebx << std::endl;
			log << "\tEcx: 0x" << std::hex << ep->ContextRecord->Ecx << std::endl;
			log << "\tEdx: 0x" << std::hex << ep->ContextRecord->Edx << std::endl;
			log << "\tEsp: 0x" << std::hex << ep->ContextRecord->Esp << std::endl;
			log << "\tEsi: 0x" << std::hex << ep->ContextRecord->Esi << std::endl;
			log << "\tEdi: 0x" << std::hex << ep->ContextRecord->Edi << std::endl;
			log << "- Debug Registers:" << std::endl;
			log << "\tContextFlags: 0x" << std::hex << ep->ContextRecord->ContextFlags << std::endl;
			log << "\tDR0: 0x" << std::hex << ep->ContextRecord->Dr0 << std::endl;
			log << "\tDR1: 0x" << std::hex << ep->ContextRecord->Dr1 << std::endl;
			log << "\tDR2: 0x" << std::hex << ep->ContextRecord->Dr2 << std::endl;
			log << "\tDR3: 0x" << std::hex << ep->ContextRecord->Dr3 << std::endl;
			log << "\tDR6: 0x" << std::hex << ep->ContextRecord->Dr6 << std::endl;
			log << "\tDR7: 0x" << std::hex << ep->ContextRecord->Dr7 << std::endl;
			log << "- Segment Registers & CPU flags:" << std::endl;
			log << "\tSegCs: 0x" << std::hex << ep->ContextRecord->SegCs << std::endl;
			log << "\tSegDs: 0x" << std::hex << ep->ContextRecord->SegDs << std::endl;
			log << "\tSegEs: 0x" << std::hex << ep->ContextRecord->SegEs << std::endl;
			log << "\tSegFs: 0x" << std::hex << ep->ContextRecord->SegFs << std::endl;
			log << "\tSegGs: 0x" << std::hex << ep->ContextRecord->SegGs << std::endl;
			log << "\tSegSs: 0x" << std::hex << ep->ContextRecord->SegSs << std::endl;
			log << "\tEFlags: 0x" << std::hex << ep->ContextRecord->EFlags << std::endl;
			log << "- Stack: " << std::hex << ep->ContextRecord->Esp << std::endl;

			for (int i = 0; i < 0x1000; i += 0x4)
			{
				auto read_val = *(uintptr_t*)(ep->ContextRecord->Esp + i);

				if (game_mod.contains_address(read_val) || mod_mod.contains_address(read_val))
				{
					wchar_t stack_ptr_mod_name[256] { 0 };
				
					const auto absolute_read_val = read_val;

					if (auto stack_ptr_mod = get_module_info_if_valid(read_val, stack_ptr_mod_name))
						log << "\t*0x" << std::hex << ep->ContextRecord->Esp + i << ": " << stack_ptr_mod_name << " + 0x" << std::hex << read_val << " (0x" << std::hex << absolute_read_val << ")" << std::endl;
				}
			}

			log.close();

			buffer += std::format(L"Exception Address: {} + {:#x}", mod_name, eip);

			SetWindowTextW(GetDlgItem(dialog, IDC_INFO_EDIT), buffer.c_str());

			while (!g_can_close_crash_wnd)
			{
				if (MSG msg; PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) && GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			DestroyWindow(dialog);

			std::exit(0);
		}

		log(RED, "Continuing exception search...");

		return EXCEPTION_CONTINUE_SEARCH;
	}
}

INT_PTR __stdcall crash_wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
	switch (msg)
	{
	case WM_COMMAND:
	{
		switch (auto param = LOWORD(w_param))
		{
		case IDOK:
		case IDCANCEL:
			g_can_close_crash_wnd = true;
			return 1;
		}
	}
	}

	return 0;
}

long __stdcall global_veh(_EXCEPTION_POINTERS* ep)
{
	return jc::bug_ripper::show_and_dump_crash(ep);
}