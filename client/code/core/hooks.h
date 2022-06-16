#pragma once

#include "protos.h"

namespace jc::hooks
{
	inline std::unordered_map<uint32_t, std::pair<void*, void*>> originals;

	inline std::atomic_int signals = 0;

	struct HookLock
	{
		HookLock() { ++signals; }
		~HookLock() { --signals; }
	};

	inline bool init()
	{
		return (MH_Initialize() == MH_OK);
	}

	inline bool destroy()
	{
		log(YELLOW, "Waiting for {} signals", signals.load());

		while (signals > 0)
			SwitchToThread();

		log(GREEN, "All signals were detected");

		return (MH_Uninitialize() == MH_OK);
	}

	template <typename T>
	inline bool hook(void* hk_fn)
	{
		void* o_fn = nullptr;

		const auto address = BITCAST(void*, T::ADDRESS);

		const bool ok = MH_CreateHook(address, hk_fn, BITCAST(void**, &o_fn)) == MH_OK;

		check(originals.insert({ T::HASH, { address, o_fn } }).second, "Cannot insert the same type of hook again");

		return ok && (MH_EnableHook(address) == MH_OK);
	}

	template <typename T>
	inline bool vhook(void* instance, void* hk_fn)
	{
		void* o_fn = nullptr;

		const auto vtable	  = read<void*>(instance);
		const auto fn_address = read<void*>(vtable, T::INDEX * 0x4);

		const bool ok = MH_CreateHook(fn_address, hk_fn, BITCAST(void**, &o_fn)) == MH_OK;

		check(originals.insert({ T::HASH, { fn_address, o_fn } }).second, "Cannot insert the same type of hook again");

		return ok && (MH_EnableHook(fn_address) == MH_OK);
	}

	template <typename T>
	inline bool unhook()
	{
		if constexpr (T::hook_type == Detour)
			return MH_DisableHook(BITCAST(void*, T::ADDRESS)) == MH_OK;
		else
		{
			auto it = originals.find(T::HASH);

			check(it != originals.end(), "Cannot find original function");

			return MH_DisableHook(it->second.first) == MH_OK;
		}
	}

	template <typename T, typename... A>
	inline auto call(A... args)
	{
		auto it = originals.find(T::HASH);

		check(it != originals.end(), "Cannot find original function");

		return RCAST(T::TYPE, it->second.second)(args...);
	}
}