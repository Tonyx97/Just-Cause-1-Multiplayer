#pragma once

namespace jc::player_global_info
{
	static constexpr uint32_t SINGLETON = 0xD32EF8; // PlayerGlobalInfo*

	static constexpr uint32_t PLAYER_HANDLE_BASE = 0x4; // PlayerHandle*
}

class CharacterHandleBase;

class PlayerGlobalInfo
{
public:

	void init();
	void destroy();

	CharacterHandleBase* get_localplayer_handle_base() const;
};

inline Singleton<PlayerGlobalInfo, jc::player_global_info::SINGLETON> g_player_global_info;