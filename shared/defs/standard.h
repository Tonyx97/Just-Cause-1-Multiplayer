#pragma once

#include <windows.h>
#include <windowsx.h>
#include <dbghelp.h>
#include <tlhelp32.h>

#include <filesystem>
#include <concepts>
#include <fstream>
#include <functional>
#include <future>
#include <intrin.h>
#include <map>
#include <mutex>
#include <deque>
#include <numbers>
#include <regex>
#include <set>
#include <source_location>
#include <stdint.h>
#include <thread>
#include <chrono>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include <prof/prof.h>

#include <memory/defs.h>
#include <memory/memory.h>

#include <utils/utils.h>

#include <math/math.h>

#ifdef JC_CLIENT
#define FAST_LOAD 1

#include "main.h"

#include <d3dx9math.h>

#include <MinHook.h>

#include <stl/ref.h>
#include <stl/vec.h>
#include <stl/map.h>
#include <stl/string.h>

#include <core/hooks.h>
#include <core/patching.h>
#elif defined(JC_SERVER)
#endif

#include <bug_ripper/bug_ripper.h>

using namespace std::chrono_literals;