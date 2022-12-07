#include "dllmain.h"

#include <initguid.h>

HMODULE hm;

bool iequals(std::wstring_view s1, std::wstring_view s2)
{
    std::wstring str1(std::move(s1));
    std::wstring str2(std::move(s2));
    std::transform(str1.begin(), str1.end(), str1.begin(), [](wchar_t c) { return ::towlower(c); });
    std::transform(str2.begin(), str2.end(), str2.begin(), [](wchar_t c) { return ::towlower(c); });
    return (str1 == str2);
}

std::wstring to_wstring(std::string_view cstr)
{
    std::string str(std::move(cstr));
    auto charsReturned = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(charsReturned, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], charsReturned);
    return wstrTo;
}

std::wstring SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken)
{
    std::wstring r;
    WCHAR* szSystemPath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(rfid, dwFlags, hToken, &szSystemPath)))
    {
        r = szSystemPath;
    }
    CoTaskMemFree(szSystemPath);
    return r;
}

HMODULE LoadLibraryW(const std::wstring& lpLibFileName)
{
    return LoadLibraryW(lpLibFileName.c_str());
}

std::wstring GetCurrentDirectoryW()
{
    static constexpr auto INITIAL_BUFFER_SIZE = MAX_PATH;
    static constexpr auto MAX_ITERATIONS = 7;
    std::wstring ret;
    auto bufferSize = INITIAL_BUFFER_SIZE;
    for (size_t iterations = 0; iterations < MAX_ITERATIONS; ++iterations)
    {
        ret.resize(bufferSize);
        auto charsReturned = GetCurrentDirectoryW(bufferSize, &ret[0]);
        if (charsReturned < ret.length())
        {
            ret.resize(charsReturned);
            return ret;
        }
        else
        {
            bufferSize *= 2;
        }
    }
    return L"";
}

std::wstring GetSelfName()
{
    const std::wstring moduleFileName = GetModuleFileNameW(hm);
    return moduleFileName.substr(moduleFileName.find_last_of(L"/\\") + 1);
}

static LONG OriginalLibraryLoaded = 0;
static LONG LoadedPluginsYet = 0;

void LoadOriginalLibrary()
{
    if (_InterlockedCompareExchange(&OriginalLibraryLoaded, 1, 0) != 0) return;

    auto szSelfName = GetSelfName();
    auto szSystemPath = SHGetKnownFolderPath(FOLDERID_System, 0, nullptr) + L'\\' + szSelfName;
    auto szLocalPath = GetModuleFileNameW(hm); szLocalPath = szLocalPath.substr(0, szLocalPath.find_last_of(L"/\\") + 1);

	if (iequals(szSelfName, L"dinput8.dll"))
		dinput8.LoadOriginalLibrary(LoadLibraryW(szSystemPath));
	else
    {
        MessageBox(0, TEXT("This library isn't supported."), TEXT("ASI Loader"), MB_ICONERROR);
        ExitProcess(0);
    }
}

void LoadPlugins()
{
    auto oldDir = GetCurrentDirectoryW(); // store the current directory

    auto szSelfPath = GetModuleFileNameW(hm).substr(0, GetModuleFileNameW(hm).find_last_of(L"/\\") + 1);
    SetCurrentDirectoryW(szSelfPath.c_str());

    LoadLibraryW(L"jcmp_client.dll");

    SetCurrentDirectoryW(oldDir.c_str()); // Reset the current directory
}

void LoadEverything()
{
    if (_InterlockedCompareExchange(&LoadedPluginsYet, 1, 0) != 0) return;

    LoadOriginalLibrary();
    LoadPlugins();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        hm = hModule;
		LoadEverything();
    }

    return TRUE;
}