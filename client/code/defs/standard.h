#pragma once

#include <windows.h>
#include <windowsx.h>
#include <dbghelp.h>
#include <tlhelp32.h>

#include <filesystem>
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

#include <d3dx9math.h>

#include <MinHook.h>

#include "global_types.h"

#include <prof/prof.h>

#include <memory/defs.h>
#include <memory/memory.h>

#include <math/math.h>

#include <stl/ref.h>

#include <stl/vec.h>
#include <stl/maps.h>
#include <stl/string.h>

#include <core/hooks.h>
#include <core/patching.h>

template <typename T>
using vec = std::vector<T>;

using namespace std::chrono_literals;