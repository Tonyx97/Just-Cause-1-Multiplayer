#pragma once

namespace jc::game_ctx
{
	static constexpr uint32_t SINGLETON				= 0xD32A78;			// GameContext*

	static constexpr uint32_t SCREEN_SIZE_X			= 0x8;				// int32_t
	static constexpr uint32_t SCREEN_SIZE_Y			= 0xC;				// int32_t
	static constexpr uint32_t UNKNOWN				= 0x1C;				// uint32_t
}

class GameContext
{
public:

	void init();
	void destroy();
	void set_window_resolution(int32_t x, int32_t y);
	void set_fullscreen(bool enabled);
};

inline Singleton<GameContext, jc::game_ctx::SINGLETON> g_game_ctx;