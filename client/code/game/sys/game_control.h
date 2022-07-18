#pragma once
#include <defs/standard.h>

namespace jc::game_control
{
	static constexpr uint32_t SINGLETON = 0xD37380; // GameControl*

	namespace fn
	{
		static constexpr uint32_t CREATE_OBJECT		= 0x4EE350;
		static constexpr uint32_t ENABLE_OBJECT		= 0x4EE480;
	}
}

class GameControl
{
public:

	void init();
	void destroy();
	void hook_tick();
	void unhook_tick();

	template <typename T, typename R = ref<T>>
	R create_object(bool enable_now = true)
	{
		R r;

		jc::stl::string class_name = T::CLASS_NAME();

		jc::this_call(jc::game_control::fn::CREATE_OBJECT, this, &r, &class_name, enable_now);

		return r;
	}

	template <typename T>
	void enable_object(ref<T>& rf)
	{
		rf.make_valid();

		jc::this_call(jc::game_control::fn::ENABLE_OBJECT, this, rf.to_holder());
	}
};

inline GameControl* g_game_control = nullptr;