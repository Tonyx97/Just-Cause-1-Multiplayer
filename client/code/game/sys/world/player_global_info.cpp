#include <defs/standard.h>

#include "player_global_info.h"

void PlayerGlobalInfo::init()
{
}

void PlayerGlobalInfo::destroy()
{
}

CharacterController* PlayerGlobalInfo::get_local_controller() const
{
	return jc::read<CharacterController*>(this, jc::player_global_info::PLAYER_CONTROLLER);
}