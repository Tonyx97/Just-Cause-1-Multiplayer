#include "defs/standard.h"

#include <registry/registry.h>

uint32_t get_pid(const wchar_t* name)
{
	auto snap_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	std::unique_ptr<HANDLE, std::function<void(HANDLE*)>> snap(&snap_handle, [](HANDLE* h) -> void { CloseHandle(*h); });

	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(*snap.get(), &entry))
		do {
			if (!wcscmp(entry.szExeFile, name))
				return entry.th32ProcessID;

		} while (Process32NextW(*snap.get(), &entry));

		return 0;
}

bool inject_dll(uint32_t pid, const std::wstring& dll_path)
{
	HANDLE token;
	TOKEN_PRIVILEGES tkp;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
	{
		LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(token, 0, &tkp, sizeof(tkp), nullptr, nullptr);
	}

	size_t dll_size = (dll_path.length() * 2) + 2;

	HANDLE game_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (!game_handle || game_handle == INVALID_HANDLE_VALUE)
		return logb(RED, "Injection error 1");

	auto allocated_mem = VirtualAllocEx(game_handle, nullptr, dll_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!allocated_mem)
		return logb(RED, "Injection error 2");

	if (!WriteProcessMemory(game_handle, allocated_mem, dll_path.c_str(), dll_size, 0))
		return logb(RED, "Injection error 3");

	HANDLE thread = CreateRemoteThread(game_handle, nullptr, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary(L"kernel32.dll"), "LoadLibraryW"), allocated_mem, 0, nullptr);

	if (!thread)
		return logb(RED, "Injection error 4");

	WaitForSingleObject(thread, INFINITE);

	DWORD ret = 0;

	GetExitCodeThread(thread, &ret);

	CloseHandle(thread);
	CloseHandle(game_handle);

	return true;
}

std::string get_current_path()
{
	char path[_MAX_PATH + 1];
	GetModuleFileNameA(NULL, path, _MAX_PATH);
	auto str = std::string(path);
	return str.substr(0, str.find_last_of("\\/") + 1);
}

std::string get_current_parent_path()
{
	char path[_MAX_PATH + 1];
	GetModuleFileNameA(NULL, path, _MAX_PATH);
	auto str = std::string(path);
	return str.substr(0, str.find_last_of("\\/") + 1) + "..\\";
}

int __stdcall browse_game_path_proc(HWND hwnd, UINT msg, LPARAM lparam, LPARAM data)
{
	if (msg == BFFM_INITIALIZED)
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, data);

	return 0;
}

int main()
{
	init("JC:MP Launcher");

	std::wstring game_dir;

	g_registry.init();

	auto load_game = [&](const std::string& ip)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		memset(&si, 0, sizeof(si));
		memset(&pi, 0, sizeof(pi));

		si.cb = sizeof(si);

		const auto current_parent_path = get_current_parent_path() + "client\\";

		const std::wstring full_game_path	= game_dir + L"JustCause.exe",
						   current_dll_path = std::wstring(current_parent_path.begin(), current_parent_path.end()) + L"jcmp_client.dll";

		log(CYAN, "Launching Just Cause...");

		bool suspend = !!GetAsyncKeyState(VK_F2);
		
		wprintf(L"Path: %s\n", game_dir.c_str());

		if (!CreateProcessW(full_game_path.c_str(), nullptr, nullptr, nullptr, FALSE, suspend ? CREATE_SUSPENDED : 0, nullptr, game_dir.c_str(), &si, &pi))
			return logb(RED, "Could not launch the game");

		if (suspend)
		{
			log(CYAN, "Game is suspended, press F3 to continue");

			while (!GetAsyncKeyState(VK_F3))
				Sleep(100);

			ResumeThread(pi.hThread);
		}

		log(CYAN, "Just Cause launched");
		
		if (!inject_dll(pi.dwProcessId, current_dll_path))
			return logb(RED, "Injection failed");

		log(CYAN, "JCMP initialized");

		return true;
	};

	auto load_ini = [&]() 
	{
		auto ini_path = get_current_path() + "loader.ini";
		
		if (!std::filesystem::exists(ini_path))
		{
			std::ofstream basic_ini(get_current_path() + "loader.ini");

			basic_ini << "[game]" << std::endl;
			basic_ini << "path = TYPE HERE THE PATH" << std::endl;

			return logb(RED, "Could not find loader.ini");
		}
		
		INIReader reader(ini_path);

		if (reader.ParseError() < 0)
			return logb(RED, "Error parsing loader.ini");
		
		auto game_dir_key = reader.Get("game", "path", "UNDEFINED");
		auto ip = reader.Get("game", "ip", "UNDEFINED");

		g_registry.set_string("ip", ip);

		if (GetFileAttributesA(game_dir_key.c_str()) == INVALID_FILE_ATTRIBUTES)
			return logb(RED, "Invalid JC directory path: \"{}\"", game_dir_key);

		game_dir = std::wstring(game_dir_key.begin(), game_dir_key.end());

		return true;
	};

	if (const auto pid = get_pid(L"JustCause.exe"))
	{
		const auto current_parent_path = get_current_parent_path() + "client\\";

		const std::wstring current_dll_path = std::wstring(current_parent_path.begin(), current_parent_path.end()) + L"jcmp_client.dll";

		if (!inject_dll(pid, current_dll_path))
			return logb(RED, "Injection failed");
	}
	else
	{
		if (!load_ini())
			return 69;
		load_game("");
	};

	g_registry.destroy();

	//std::this_thread::sleep_for(std::chrono::seconds(1));

	return 0;
}