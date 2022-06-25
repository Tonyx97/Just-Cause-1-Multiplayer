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

void Player::set_nick(const std::string& v)
{
	static_info.nick = v;
}

void Player::set_skin(uint32_t v)
{
	static_info.skin = v;

#ifdef JC_CLIENT

#endif
}

uint32_t Player::get_skin() const
{
	return static_info.skin;
}