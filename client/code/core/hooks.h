#pragma once

#include <Windows.h>
#include <DbgHelp.h>
#include <utils/utils.h>

#include <game/globals.h>
#include <game/sys/physics.h>

class Transform;
class Character;
class GameControl;
class GameStatus;

inline std::map<uint32_t, std::vector<void*>> pending_hooks;
inline std::map<uint32_t, std::vector<void*>> placed_hooks;

template <typename T>
struct DetourHook
{
	T original = nullptr,
	  target = nullptr;

	void* hk = nullptr;

	bool created = false;

	DetourHook(ptr address, void* hk) : target(BITCAST(T, address)), hk(hk) {}

	inline void hook()
	{
		if (created)
			return;

		const auto status = MH_CreateHook(BITCAST(void*, target), hk, BITCAST(void**, &original));

		created = (status == MH_OK);

		check(created && original, "Hook could not be created {}", int(status));

		pending_hooks[uint32_t(GetCurrentThreadId())].push_back(BITCAST(void*, target));
	}

	inline void unhook()
	{
		//hooks_to_destroy.push_back(BITCAST(void*, target));
	}

	template <typename... A>
	__declspec(noinline) auto call(A... args)
	{
		check(original, "Hook was not applied, invalid original function");

		return original(args...);
	}

	operator bool() const { return created; }
};

#define HOOK_FIRST_ARG_(x, ...) x
#define HOOK_FIRST_ARG(args)	HOOK_FIRST_ARG_ args

#define HOOK_REST_ARGS_IMPL(first, ...) __VA_ARGS__
#define HOOK_REST_ARGS(args)			HOOK_REST_ARGS_IMPL args

#define DECLARE_HOOK_FN(name, ret, conv, ...)		ret conv hk_##name(__VA_ARGS__);
#define DEFINE_HOOK_FN(name, ret, conv, ...)		ret conv hk_##name(__VA_ARGS__)
#define DECLARE_HOOK(name, address, ret, conv, ...) DetourHook<ret(conv*)(__VA_ARGS__)> name##_hook(address, &hk_##name);

#define DEFINE_HOOK_IMPL(name, address, ret, conv, ...)	DECLARE_HOOK_FN(name, ret, conv, __VA_ARGS__) \
														DECLARE_HOOK(name, address, ret, conv, __VA_ARGS__) \
														DEFINE_HOOK_FN(name, ret, conv, __VA_ARGS__)

#define DEFINE_HOOK_THISCALL_IMPL(name, address, ret, ...)	DECLARE_HOOK_FN(name, ret, __fastcall, HOOK_FIRST_ARG((__VA_ARGS__)), void*, HOOK_REST_ARGS((__VA_ARGS__))) \
															DECLARE_HOOK(name, address, ret, __thiscall, __VA_ARGS__)\
															DEFINE_HOOK_FN(name, ret, __fastcall, HOOK_FIRST_ARG((__VA_ARGS__)), void*, HOOK_REST_ARGS((__VA_ARGS__)))

#define DEFINE_HOOK_FASTCALL(name, address, ret, ...)			DEFINE_HOOK_IMPL(name, address, ret, __fastcall, __VA_ARGS__)
#define DEFINE_HOOK_STDCALL(name, address, ret, ...)			DEFINE_HOOK_IMPL(name, address, ret, __stdcall, __VA_ARGS__)
#define DEFINE_HOOK_CCALL(name, address, ret, ...)				DEFINE_HOOK_IMPL(name, address, ret, __cdecl, __VA_ARGS__)
#define DEFINE_HOOK_THISCALL(name, address, ret, ...)			DEFINE_HOOK_THISCALL_IMPL(name, address, ret, __VA_ARGS__)

#define DEFINE_HOOK_THISCALL_S(name, address, ret, arg)			DECLARE_HOOK_FN(name, ret, __fastcall, arg, void*) \
																DECLARE_HOOK(name, address, ret, __thiscall, arg)\
																DEFINE_HOOK_FN(name, ret, __fastcall, arg, void*)

namespace jc::hooks
{
	void init();
	void destroy();
	void hook_queued();
	void unhook_queued();
	void hook_game_fns();
	void unhook_game_fns();
}