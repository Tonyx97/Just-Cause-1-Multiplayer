#include <defs/standard.h>

#include "player_global_info.h"

void PlayerGlobalInfo::init()
{
}

void PlayerGlobalInfo::destroy()
{
}

CharacterHandleBase* PlayerGlobalInfo::get_localplayer_handle_base() const
{
	return jc::read<CharacterHandleBase*>(this, jc::player_global_info::PLAYER_HANDLE_BASE);
}