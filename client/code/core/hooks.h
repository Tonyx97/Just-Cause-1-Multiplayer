#pragma once

#include <utils/utils.h>

#include <game/globals.h>

#include <core/patching.h>

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

	inline void hook(bool enable)
	{
		if (enable)
			hook();
		else unhook();
	}

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
	__forceinline auto call(A... args)
	{
		check(original, "Hook was not applied, invalid original function");

		return original(args...);
	}

	template <typename... A>
	__forceinline auto operator()(A... args)
	{
		return call(args...);
	}

	operator bool() const { return created; }
};

struct InlineHook
{
public:

	struct Parameter
	{
		ptr esp = 0u,
			ebp = 0u;

		template <typename T>
		T read_ebp(int offset)
		{
			return jc::read<T>(ebp, -offset);
		}

		template <typename T>
		T* at_ebp(int offset)
		{
			return REF(T*, ebp, -offset);
		}

		template <typename T>
		T read_esp(int offset)
		{
			return jc::read<T>(esp, offset);
		}

		template <typename T>
		T* at_esp(int offset)
		{
			return REF(T*, esp, offset);
		}
	};

private:

	patch to_shell_patch {},
		  optional_jmp_patch {};

	Parameter parameter{};

	ptr address = 0u,
		hk = 0u;

	uint8_t* shell_base = nullptr;

public:

	template <typename Tx, typename Ty>
	InlineHook(Tx address, Ty hk) : address(BITCAST(ptr, address)), hk(BITCAST(ptr, hk)) {}

	~InlineHook()
	{
	}

	void hook(bool v, ptr optional_jmp_target = 0)
	{
		if (v)
			hook(optional_jmp_target);
		else unhook();
	}

	void hook(ptr optional_jmp_target = 0)
	{
		hde32s hde;

		size_t original_code_len = 0u;

		// get all following instructions until we reached a size of 5 to fit
		// the jump which is 5 bytes

		while (int len = hde32_disasm(BITCAST(void*, address + original_code_len), &hde))
			if ((original_code_len += len) >= 5)
				break;

		uint8_t shell_bytecode[] =
		{
			0x68, 0x00, 0x00, 0x00, 0x00,	// push <return address>
			0x50,							// push eax
			0xB8, 0x00, 0x00, 0x00, 0x00,	// mov eax, <parameter>
			0x89, 0x20,						// mov [eax], esp
			0x89, 0x68, 0x04,				// mov [eax + 0x4], ebp
			0x60,							// pushad
			0x9C,							// pushf
			0x50,							// push eax
			0xE8, 0x00, 0x00, 0x00, 0x00,	// call <hk>
			0x58,							// pop eax
			0x9D,							// popf
			0x61,							// popad
			0x58,							// pop eax
			0xC3,							// ret
		};

		const auto shellcode_size = sizeof(shell_bytecode);
		const auto total_shellcode_size = original_code_len + shellcode_size;
		const auto shellcode_parameter_offset = 6;
		const auto shellcode_call_offset = 19;

		// allocate original code and shell memory

		shell_base = new uint8_t[total_shellcode_size];

		const auto orginal_code_address = shell_base;
		const auto shell_address = shell_base + original_code_len;

		// write return address at the beginning of the shell
		// which will be pushed to use ret later

		*(ptr*)(shell_bytecode + 1) = address + original_code_len;

		// write the paremeter address in the shell

		*(ptr*)(shell_bytecode + shellcode_parameter_offset + 1) = BITCAST(ptr, this) + offsetof(InlineHook, parameter);

		// write call offset from the shell call to our hook

		*(ptr*)(shell_bytecode + shellcode_call_offset + 1) = jc::calc_call_offset(shell_address + shellcode_call_offset, hk).value;

		// copy original code and the shell code into the shell memory

		memcpy(orginal_code_address, BITCAST(void*, address), original_code_len);
		memcpy(shell_address, shell_bytecode, shellcode_size);

		// write jump in the target address

		to_shell_patch.jump(address, shell_base);

		// write optional jump so we can skip original code

		if (optional_jmp_target != 0)
			optional_jmp_patch.jump(address + original_code_len, optional_jmp_target);
	}

	void unhook()
	{
		if (!shell_base)
			return;

		optional_jmp_patch._undo();
		to_shell_patch._undo();

		delete[] shell_base;

		shell_base = nullptr;
	}
};

// detour hooks

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

// inline hooks

// all inline hooks are cdecl
// ihp = the parameter for all inline hooks containing ESP and EBP
//
#define DEFINE_INLINE_HOOK_IMPL(name, address)	void __cdecl hk_##name(InlineHook::Parameter*); \
												InlineHook name##_hook(address, &hk_##name); \
												void __cdecl hk_##name(InlineHook::Parameter* ihp)

namespace jc::hooks
{
	void init();
	void destroy();
	void hook_queued();
	void unhook_queued();
	void hook_game_fns(bool enable);
}