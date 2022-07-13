#include <defs/standard.h>

#include "object_lists.h"

#include <shared_mp/player_client/player_client.h>
#include <shared_mp/objs/player.h>

#ifdef JC_CLIENT
PlayerClient* ObjectLists::add_player_client(NID nid)
{
	const auto pc = CREATE_PLAYER_CLIENT(nid);

	check(add_net_object(pc->get_player()), "Could not add a player client");

	return pc;
}

Player* ObjectLists::get_player_by_character(Character* character)
{
	for (const auto& [nid, player] : players)
		if (player->get_character() == character)
			return player;

	return nullptr;
}
#else
PlayerClient* ObjectLists::add_player_client(ENetPeer* peer)
{
	const auto pc = CREATE_PLAYER_CLIENT(peer);

	peer->data = pc;

	check(add_net_object(pc->get_player()), "Could not add a player client");

	return pc;
}
#endif

bool ObjectLists::remove_player_client(PlayerClient* pc)
{
	check(pc, "Invalid player client at '{}'", CURR_FN);

	remove_net_object(pc->get_player());

	DESTROY_PLAYER_CLIENT(pc);

	return true;
}

NetObject* ObjectLists::get_net_object_by_nid_impl(NID nid)
{
	auto it = net_objects.find(nid);
	return it != net_objects.end() ? it->second : nullptr;
}

NetObject* ObjectLists::add_net_object(NetObject* net_obj)
{
	const auto nid = net_obj->get_nid();

	switch (net_obj->get_type())
	{
	case NetObject_Player:
	{
		player_clients.insert({ nid, net_obj->get_player_client()});
		players.insert({ nid, net_obj->cast<Player>() });
		break;
	}
	default:
		return nullptr;
	}

	net_objects.insert({ nid, net_obj });

	return net_obj;
}

bool ObjectLists::remove_net_object(NetObject* net_obj)
{
	const auto nid = net_obj->get_nid();

	switch (net_obj->get_type())
	{
	case NetObject_Player:
	{
		player_clients.erase(nid);
		players.erase(nid);
		break;
	}
	default:
		return false;
	}

	net_objects.erase(nid);

	return true;
}

Player* ObjectLists::get_player_by_nid(NID nid)
{
	auto it = players.find(nid);
	return it != players.end() ? it->second : nullptr;
}

PlayerClient* ObjectLists::get_player_client_by_nid(NID nid)
{
	auto it = player_clients.find(nid);
	return it != player_clients.end() ? it->second : nullptr;
}