#pragma once
#include <defs/standard.h>

namespace jc::game_control
{
	static constexpr uint32_t SINGLETON = 0xD37380; // GameControl*

	namespace fn
	{
		static constexpr uint32_t CREATE_OBJECT				= 0x4EE350;
		static constexpr uint32_t DISPATCH_LOCATIONS_LOAD	= 0x4F1990;
		static constexpr uint32_t ADD_OBJECT_TO_WORLD		= 0x4EE480;
	}
}

class GameControl
{
private:
	
	void* create_object_internal(void* r, jc::stl::string* class_name, bool add_to_world);

public:

	void init();
	void destroy();
	void create_global_objects();
	void hook_create_object();
	void unhook_create_object();
	void dispatch_locations_load();
	void on_tick();

	template <typename T>
	shared_ptr<T> create_object(bool add_to_world = true)
	{
		shared_ptr<T> r;

		jc::stl::string class_name = T::CLASS_NAME();

		create_object_internal(&r, &class_name, add_to_world);

		return r;
	}

	template <typename T>
	void add_object_to_world(shared_ptr<T>& obj)
	{
		jc::this_call(jc::game_control::fn::ADD_OBJECT_TO_WORLD, this, weak_ptr<T>(obj));
	}
};

inline Singleton<GameControl, jc::game_control::SINGLETON> g_game_control;