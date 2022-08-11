#pragma once

namespace jc::time_system
{
	static constexpr uint32_t SINGLETON = 0xAF2368; // TimeSystem*

	static constexpr uint32_t FPS		 = 0xC; // float
	static constexpr uint32_t DELTA		 = 0x10; // float
	static constexpr uint32_t LAST_FPS	 = 0x14; // float
	static constexpr uint32_t LAST_DELTA = 0x18; // float
	static constexpr uint32_t TIME_SCALE = 0x1C; // float
}

class TimeSystem
{
public:
	void init();
	void destroy();

	void set_time_scale(float v);

	float get_fps() const;
	float get_delta() const;
	float get_last_fps() const;
	float get_last_delta() const;
	float get_time_scale() const;
};

inline Singleton<TimeSystem, jc::time_system::SINGLETON> g_time;