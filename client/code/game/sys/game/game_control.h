#pragma once
#include <defs/standard.h>

namespace jc::game_control
{
	static constexpr uint32_t SINGLETON = 0xD37380; // GameControl*

	namespace fn
	{
		static constexpr uint32_t CREATE_OBJECT				= 0x4EE350;
		static constexpr uint32_t DISPATCH_LOCATIONS_LOAD	= 0x4F1990;
		static constexpr uint32_t ENABLE_OBJECT				= 0x4EE480;
	}
}

class GameControl
{
private:
	
	void* create_object_internal(void* r, jc::stl::string* class_name, bool enable_now);

public:

	void init();
	void destroy();
	void create_global_objects();
	void hook_create_object();
	void unhook_create_object();
	void dispatch_locations_load();
	void on_tick();

	template <typename T, typename R = ref<T>>
	R create_object(bool enable_now = true)
	{
		R r;

		jc::stl::string class_name = T::CLASS_NAME();

		create_object_internal(&r, &class_name, enable_now);

		return r;
	}

	template <typename T>
	void enable_object(ref<T>& rf)
	{
		rf.make_valid();

		jc::this_call(jc::game_control::fn::ENABLE_OBJECT, this, rf.to_holder());
	}
};

inline Singleton<GameControl, jc::game_control::SINGLETON> g_game_control;