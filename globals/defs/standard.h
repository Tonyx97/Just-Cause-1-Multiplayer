#pragma once

#include <windows.h>
#include <windowsx.h>
#include <dbghelp.h>
#include <tlhelp32.h>

#include <iostream>
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
#include <stack>
#include <any>

#include <prof/prof.h>

#include <memory/defs.h>
#include <memory/memory.h>

#include <utils/utils.h>

#if defined(JC_CLIENT) || defined(JC_SERVER) || defined(JC_ASSET_CONV)
#include <math/math.h>
#endif

#ifdef JC_CLIENT
#include "main.h"

#include <d3dx9math.h>

#include <MinHook.h>
#include <hde32.h>

#include <stl/ref.h>
#include <stl/vec.h>
#include <stl/map.h>
#include <stl/string.h>

#include <core/hooks.h>

#include <../rsrc/resource.h>
#elif defined(JC_SERVER)
#endif

#include <bug_ripper/bug_ripper.h>

#include "general_purpose_macros.h"

using namespace std::chrono_literals;