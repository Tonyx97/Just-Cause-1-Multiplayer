#ifndef JC_LOADER
#include <defs/standard.h>

#include <net/interface.h>

#include "player.h"

#ifdef JC_CLIENT
Player::Player(PlayerClient* pc, NID nid) : client(pc)
{
	set_nid(nid);
	set_player_client(pc);
}
#else
Player::Player(PlayerClient* pc) : client(pc)
{
	set_player_client(pc);
}
#endif

Player::~Player()
{
}
#endif