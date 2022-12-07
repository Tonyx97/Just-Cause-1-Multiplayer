#pragma once

#include <windows.h>
#include <cctype>
#include <string>
#include <shlobj.h>
#include <stdio.h>
#include <filesystem>
#include <functional>
#include <set>
#include <intrin.h>

#include <ModuleList\ModuleList.hpp>

struct shared
{
    FARPROC DllCanUnloadNow;
    FARPROC DllGetClassObject;
    FARPROC DllRegisterServer;
    FARPROC DllUnregisterServer;
    FARPROC DebugSetMute;

    void LoadOriginalLibrary(HMODULE dll)
    {
        DllCanUnloadNow = GetProcAddress(dll, "DllCanUnloadNow");
        DllGetClassObject = GetProcAddress(dll, "DllGetClassObject");
        DllRegisterServer = GetProcAddress(dll, "DllRegisterServer");
        DllUnregisterServer = GetProcAddress(dll, "DllUnregisterServer");
        DebugSetMute = GetProcAddress(dll, "DebugSetMute");
    }
} shared;

struct dinput8_dll
{
    HMODULE dll;
    FARPROC DirectInput8Create;

    void LoadOriginalLibrary(HMODULE module)
    {
        dll = module;
        shared.LoadOriginalLibrary(dll);
        DirectInput8Create = GetProcAddress(dll, "DirectInput8Create");
    }
} dinput8;

__declspec(naked) void _DirectInput8Create() { _asm { jmp[dinput8.DirectInput8Create] } }

__declspec(naked) void _DllCanUnloadNow() { _asm { jmp[shared.DllCanUnloadNow] } }
__declspec(naked) void _DllGetClassObject() { _asm { jmp[shared.DllGetClassObject] } }
__declspec(naked) void _DllRegisterServer() { _asm { jmp[shared.DllRegisterServer] } }
__declspec(naked) void _DllUnregisterServer() { _asm { jmp[shared.DllUnregisterServer] } }
__declspec(naked) void _DebugSetMute() { _asm { jmp[shared.DebugSetMute] } }