#include <defs/standard.h>

#include "player_client.h"

PlayerClient::PlayerClient()
{
}

PlayerClient::~PlayerClient()
{
}

void PlayerClient::set_nick(const std::string& v)
{
	nick = v;
}