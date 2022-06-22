#pragma once

#ifndef JC_LOADER
#include "net_obj.h"

#include <net/interface.h>

class ObjectLists
{
private:
	std::unordered_map<NID, PlayerClient*> player_clients;

public:

#ifdef JC_CLIENT
	PlayerClient* add_player_client(NID nid);

	void remove_player_client(PlayerClient* v);
#else
	PlayerClient* add_player_client(ENetEvent& e);

	void remove_player_client(ENetEvent& e);
#endif

	size_t get_player_client_count() const { return player_clients.size(); }

	PlayerClient* get_player_client_by_nid(NID nid);
};
#endif