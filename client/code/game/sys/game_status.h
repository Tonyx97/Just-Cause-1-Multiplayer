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

	bool is_in_game() const { return get_status() == GameStatus_InGame; }
	bool is_game_loaded() const;

	uint32_t get_status() const;

	static bool __fastcall hk_dispatch(GameStatus* gs);
};

inline GameStatus* g_game_status = nullptr;