#pragma once

namespace jc::game_status
{
	static constexpr uint32_t SINGLETON = 0xD32F70; // GameStatus*

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

	bool is_in_game() const { return get_status() == GameStatus_InGame; }

	uint32_t get_status() const;
};

inline GameStatus* g_game_status = nullptr;