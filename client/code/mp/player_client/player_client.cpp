#include <defs/standard.h>

#include "player_client.h"

PlayerClient::PlayerClient(NID nid)
{
	set_nid(nid);
}

PlayerClient::~PlayerClient()
{
}

void PlayerClient::set_nick(const std::string& v)
{
	nick = v;
}