#pragma once

namespace jc::player_global_info
{
	static constexpr uint32_t SINGLETON = 0xD32EF8; // PlayerGlobalInfo*

	static constexpr uint32_t PLAYER_CONTROLLER = 0x4; // CharacterController*
}

class CharacterController;

class PlayerGlobalInfo
{
public:

	void init();
	void destroy();

	CharacterController* get_local_controller() const;
};

inline Singleton<PlayerGlobalInfo, jc::player_global_info::SINGLETON> g_player_global_info;