#pragma once

#include <utils/utils.h>

#include <game/globals.h>
#include <game/sys/physics.h>

class Transform;
class Character;
class GameControl;
class GameStatus;

template <typename type, ptr address>
struct prototype
{
	using TYPE = type;

	static constexpr ptr ADDRESS = address;
};

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
		created = (MH_CreateHook(BITCAST(void*, target), hk, BITCAST(void**, &original)) == MH_OK);

		check(created, "Hook could not be created");

		MH_QueueEnableHook(BITCAST(void*, target));
	}

	inline void unhook()
	{
		MH_QueueDisableHook(BITCAST(void*, target));
	}

	template <typename... A>
	inline auto call(A... args)
	{
		return original(args...);
	}
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
#define DEFINE_HOOK_THISCALL(name, address, ret, ...)			DEFINE_HOOK_THISCALL_IMPL(name, address, ret, __VA_ARGS__)

#define DEFINE_HOOK_THISCALL_S(name, address, ret, arg)			DECLARE_HOOK_FN(name, ret, __fastcall, arg, void*) \
																DECLARE_HOOK(name, address, ret, __thiscall, arg)\
																DEFINE_HOOK_FN(name, ret, __fastcall, arg, void*)

namespace jc::proto
{
	// testing

	/*namespace dbg
	{
		using init_std_string = prototype<int(__thiscall*)(std::string*, const char*, int), jc::string::fn::INIT>;
		using print_error = prototype<int(__stdcall*)(const char*, ...), 0x40D0B0>;
		using hash_str = prototype<uint32_t(__cdecl*)(std::string*), 0x473720>;

		namespace map_dumper
		{
			using vcall6_obj_init = prototype<void(__thiscall*)(ptr, ptr), 0x6D8E30>;
			using find_int = prototype<bool(__thiscall*)(ptr, uint32_t*, int*), 0x46AEF0>;
			using find_bool = prototype<bool(__thiscall*)(ptr, uint32_t*, bool*), 0x46AF70>;
			using find_int16 = prototype<bool(__thiscall*)(ptr, uint32_t*, int16_t*), 0x4C0030>;
			using find_float = prototype<bool(__thiscall*)(ptr, uint32_t*, float*), 0x46AE70>;
			using find_vec3 = prototype<bool(__thiscall*)(ptr, uint32_t*, vec3*), 0x50E7D0>;
			using find_vec3u16 = prototype<bool(__thiscall*)(ptr, uint32_t*, u16vec3*), 0x7A96A0>;
			using find_mat4 = prototype<bool(__thiscall*)(ptr, uint32_t*, mat4*), 0x4BFFB0>;
			using find_string = prototype<bool(__thiscall*)(ptr, uint32_t*, std::string*), 0x46ADD0>;
			using find_string2 = prototype<bool(__cdecl*)(ptr, uint32_t, ptr*, std::string*), 0x987DD0>;
			using find_string3 = prototype<bool(__cdecl*)(ptr, uint32_t, ptr*, std::string*), 0x987E80>;
			using find_string4 = prototype<bool(__cdecl*)(ptr, uint32_t, ptr*, ptr*, std::string*), 0x987D20>;
			using get_mat4 = prototype<mat4* (__thiscall*)(ptr, uint32_t*), 0x46B050>;

		}

		using test = prototype<void(__thiscall*)(ptr event_instance, int userdata), 0x407FD0>;
		using test2 = prototype<void(__thiscall*)(ptr a1, int id), 0x744230>;
		// using test3 = prototype<int(__cdecl*)(int, ptr, ptr*, ptr*, std::string*), 0x987D20, utils::hash::JENKINS("test3")>;
		//  using LoadAsset = prototype<int(__thiscall*)(int, std::string*, int, const char*, int), 0x5C2DC0, utils::hash::JENKINS("test")>;
		//  using LoadModel = prototype<int(__thiscall*)(int, std::string*, Transform*, bool, bool, int), 0x57AF60, utils::hash::JENKINS("test")>;
		//  using test2 = prototype<int(__thiscall*)(int, int, Transform*, int, std::string*, int, int), 0x8ADE60, utils::hash::JENKINS("test2")>;
	}*/
}

namespace jc::hooks
{
	inline std::atomic_int signals = 0;

	struct HookLock
	{
		HookLock() { ++signals; }
		~HookLock() { --signals; }
	};

	void hook_all();
	void unhook_all();

	inline bool init()
	{
		return MH_Initialize() != MH_OK;
	}

	inline bool destroy()
	{
		log(YELLOW, "Waiting for {} signals", signals.load());

		while (signals > 0)
			SwitchToThread();

		log(GREEN, "All signals were detected");

		return (MH_Uninitialize() == MH_OK);
	}
}