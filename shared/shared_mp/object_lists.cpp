#ifndef JC_LOADER
#include <defs/standard.h>

#include "object_lists.h"

#include <shared_mp/player_client/player_client.h>

#ifdef JC_CLIENT
PlayerClient* ObjectLists::add_player_client(NID nid)
{
	const auto pc = CREATE_PLAYER_CLIENT(nid);

	player_clients.insert({ nid, pc });

	return pc;
}

void ObjectLists::remove_player_client(PlayerClient* v)
{
	check(v, "Invalid player client at '{}'", CURR_FN);

	player_clients.erase(v->get_nid());

	DESTROY_PLAYER_CLIENT(v);
}
#else
PlayerClient* ObjectLists::add_player_client(ENetEvent& e)
{
	const auto pc = CREATE_PLAYER_CLIENT(e.peer);

	e.peer->data = pc;

	player_clients.insert({ pc->get_nid(), pc });

	return pc;
}

void ObjectLists::remove_player_client(ENetEvent& e)
{
	const auto pc = AS_PC(e.peer->data);

	check(pc, "Invalid player client at '{}'", CURR_FN);

	player_clients.erase(pc->get_nid());

	DESTROY_PLAYER_CLIENT(pc);

	e.peer->data = nullptr;
}
#endif

PlayerClient* ObjectLists::get_player_client_by_nid(NID nid)
{
	auto it = player_clients.find(nid);
	return it != player_clients.end() ? it->second : nullptr;
}
#endif