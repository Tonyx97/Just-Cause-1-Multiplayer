#pragma once

namespace jc::game_status
{
	static constexpr uint32_t SINGLETON = 0xD32F70; // GameStatus*

	static constexpr uint32_t GAME_LOADED = 0x1C; // bool
	static constexpr uint32_t STATUS = 0x20; // uint32_t
}

enum GameStatusID : uint32_t
{
	GameStatus_MainMenu,
	GameStatus_StartLoad,
	GameStatus_BeginLoad,
	GameStatus_Load,
	GameStatus_InGame,
	GameStatus_Unload
};

class GameStatus
{
public:

	void init();
	void destroy();
	void hook_dispatcher();
	void unhook_dispatcher();
	void load_game();

	bool is_in_game() const { return get_status() == GameStatus_InGame; }
	bool is_game_loaded() const;

	uint32_t get_status() const;
};

inline Singleton<GameStatus, jc::game_status::SINGLETON> g_game_status;