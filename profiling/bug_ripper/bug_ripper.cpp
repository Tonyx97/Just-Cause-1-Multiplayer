#include <defs/standard.h>

#include "bug_ripper.h"

#ifdef JC_CLIENT
#include <core/ui.h>
#include <core/hooks.h>
#endif

#define FORCE_ENABLE_BUG_RIPPER 0
#define ENABLE_EXCEPTION_REROUTING 0

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

#ifdef JC_CLIENT
	DEFINE_HOOK_STDCALL(game_unhandled_exception_filter, 0x99974C, LONG, _EXCEPTION_POINTERS* ep)
	{
		// re-route the exception handling to ours

		return global_veh(ep);
	}
#endif

	bool init(void* mod_base)
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

#if defined(JC_DBG) && !FORCE_ENABLE_BUG_RIPPER
		return true;
#else
		return AddVectoredExceptionHandler(FALSE, global_veh);
#endif
	}

	bool destroy()
	{
#if defined(JC_DBG) && !FORCE_ENABLE_BUG_RIPPER
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

		wchar_t mod_name[256] { 0 };

		if (auto mod_base = get_module_info_if_valid(eip, mod_name))
		{
			static auto kernelbase = BITCAST(uintptr_t, GetModuleHandleW(L"KERNELBASE"));

			/*if (mod_base == kernelbase)
				return EXCEPTION_CONTINUE_SEARCH;*/

#ifdef JC_CLIENT
			// hide the fucking game

			const auto game_hwnd = g_ui->get_window();

			ShowWindow(game_hwnd, SW_HIDE);
			UpdateWindow(game_hwnd);
			ShowCursor(true);

			// show crash dialog

			auto dialog = CreateDialogW(mod_mod.as_module(), MAKEINTRESOURCEW(IDD_CRASH_DIALOG), nullptr, crash_wnd_proc);

			SendMessageW(dialog, WM_SETICON, ICON_SMALL, GET_GAME_ICON());

			RECT rect;

			GetWindowRect(dialog, &rect);

			const int width = rect.right - rect.left;
			const int height = rect.bottom - rect.top;

			SetWindowText(dialog, L"Just Cause Multiplayer (Bug Ripper)");
			SetWindowPos(dialog, HWND_TOPMOST, (GetSystemMetrics(SM_CXSCREEN) - width) / 2, (GetSystemMetrics(SM_CYSCREEN) - height) / 2, width, height, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			BringWindowToTop(dialog);
#endif

			std::wstring buffer = L"The game crashed, we need to gather information about this crash.\r\nPlease send us report so we can fix the issue as soon as possible.\r\nMore Information about this crash in game's logs.\r\n\r\n";
			
			if (!std::filesystem::is_directory("Crash Logs"))
				std::filesystem::create_directory("Crash Logs");

#ifdef JC_CLIENT
			std::string str_date = util::time::get_str_date();
			std::string str_time_path = util::time::get_str_time_path();
#else
			std::string str_date = std::to_string(util::rand::rand_int(0, 0x7FFFFFFF));
			std::string str_time_path = std::to_string(util::rand::rand_int(0, 0x7FFFFFFF));
#endif

			std::wofstream log_file("Crash Logs\\CRASH DUMP " + str_date + ' ' + str_time_path + ".log", std::ios::trunc);

			log_file << "- Exception Code: " << std::hex << ep->ExceptionRecord->ExceptionCode << std::endl;
			log_file << "- GP Registers:" << std::endl;
			log_file << "\tEip: " << mod_name << " + 0x" << std::hex << eip << std::endl;
			log_file << "\tEbp: 0x" << std::hex << ep->ContextRecord->Ebp << std::endl;
			log_file << "\tEax: 0x" << std::hex << ep->ContextRecord->Eax << std::endl;
			log_file << "\tEbx: 0x" << std::hex << ep->ContextRecord->Ebx << std::endl;
			log_file << "\tEcx: 0x" << std::hex << ep->ContextRecord->Ecx << std::endl;
			log_file << "\tEdx: 0x" << std::hex << ep->ContextRecord->Edx << std::endl;
			log_file << "\tEsp: 0x" << std::hex << ep->ContextRecord->Esp << std::endl;
			log_file << "\tEsi: 0x" << std::hex << ep->ContextRecord->Esi << std::endl;
			log_file << "\tEdi: 0x" << std::hex << ep->ContextRecord->Edi << std::endl;
			log_file << "- Debug Registers:" << std::endl;
			log_file << "\tContextFlags: 0x" << std::hex << ep->ContextRecord->ContextFlags << std::endl;
			log_file << "\tDR0: 0x" << std::hex << ep->ContextRecord->Dr0 << std::endl;
			log_file << "\tDR1: 0x" << std::hex << ep->ContextRecord->Dr1 << std::endl;
			log_file << "\tDR2: 0x" << std::hex << ep->ContextRecord->Dr2 << std::endl;
			log_file << "\tDR3: 0x" << std::hex << ep->ContextRecord->Dr3 << std::endl;
			log_file << "\tDR6: 0x" << std::hex << ep->ContextRecord->Dr6 << std::endl;
			log_file << "\tDR7: 0x" << std::hex << ep->ContextRecord->Dr7 << std::endl;
			log_file << "- Segment Registers & CPU flags:" << std::endl;
			log_file << "\tSegCs: 0x" << std::hex << ep->ContextRecord->SegCs << std::endl;
			log_file << "\tSegDs: 0x" << std::hex << ep->ContextRecord->SegDs << std::endl;
			log_file << "\tSegEs: 0x" << std::hex << ep->ContextRecord->SegEs << std::endl;
			log_file << "\tSegFs: 0x" << std::hex << ep->ContextRecord->SegFs << std::endl;
			log_file << "\tSegGs: 0x" << std::hex << ep->ContextRecord->SegGs << std::endl;
			log_file << "\tSegSs: 0x" << std::hex << ep->ContextRecord->SegSs << std::endl;
			log_file << "\tEFlags: 0x" << std::hex << ep->ContextRecord->EFlags << std::endl;
			log_file << "- Stack: " << std::hex << ep->ContextRecord->Esp << std::endl;

			{
				DWORD machine = IMAGE_FILE_MACHINE_I386;

				CONTEXT context{};
				context.ContextFlags = CONTEXT_FULL;

				RtlCaptureContext(&context);

				SymInitialize(GetCurrentProcess(), NULL, TRUE);
				SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS);

				STACKFRAME64 frame{};

				frame.AddrPC.Offset = ep->ContextRecord->Eip;
				frame.AddrFrame.Offset = ep->ContextRecord->Ebp;
				frame.AddrStack.Offset = ep->ContextRecord->Eip;
				frame.AddrPC.Mode = AddrModeFlat;
				frame.AddrFrame.Mode = AddrModeFlat;
				frame.AddrStack.Mode = AddrModeFlat;

				while (StackWalk64(machine, GetCurrentProcess(), GetCurrentThread(), &frame, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
				{
					wchar_t mod_name_buffer[MAX_PATH] = { 0 };

					std::wstring mod_name,
								 fn_name;

					const auto address = static_cast<uintptr_t>(frame.AddrPC.Offset);

					if (const auto mod_base = SymGetModuleBase(GetCurrentProcess(), address))
					{
						GetModuleFileNameW((HMODULE)mod_base, mod_name_buffer, MAX_PATH);

						mod_name = mod_name_buffer;

						char symbol_buffer[sizeof(IMAGEHLP_SYMBOL) + 255] = { 0 };

						PIMAGEHLP_SYMBOL symbol = (PIMAGEHLP_SYMBOL)symbol_buffer;

						symbol->SizeOfStruct = sizeof(symbol_buffer);
						symbol->MaxNameLength = MAX_PATH;

						if (SymGetSymFromAddr(GetCurrentProcess(), address, NULL, symbol))
							fn_name = util::string::convert(symbol->Name);

#ifdef JC_CLIENT
						if (mod_base == game_mod.base)
						{
							log_file << "[" << mod_name << " + 0x" << std::hex << address - mod_base << "] 0x"
								<< std::hex << address
								<< '\n';
						}
						else
#endif
						{
							log_file << "[" << mod_name << " + 0x" << std::hex << address - mod_base << "] 0x"
								<< std::hex << address << " = "
								<< fn_name << ""
								<< '\n';
						}
					}
				}
			}

			log_file.close();

			buffer += std::format(L"Exception Address: {} + {:#x}", mod_name, eip);

#ifdef JC_CLIENT
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
#endif

			std::exit(0);
		}

		log(RED, "Continuing exception search...");

		return EXCEPTION_CONTINUE_SEARCH;
	}

	bool reroute_exception_handler(bool place)
	{
#if defined(JC_CLIENT) && ENABLE_EXCEPTION_REROUTING
		if (place)
			game_unhandled_exception_filter_hook.hook();
		else  game_unhandled_exception_filter_hook.unhook();
#endif

		return true;
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