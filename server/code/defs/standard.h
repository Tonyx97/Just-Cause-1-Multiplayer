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
#include <concepts>
#include <mutex>
#include <numbers>
#include <regex>
#include <set>
#include <source_location>
#include <stdint.h>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include <prof/prof.h>

#include <memory/defs.h>
#include <memory/memory.h>

template <typename T>
using vec = std::vector<T>;

using namespace std::chrono_literals;