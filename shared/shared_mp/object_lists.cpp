#include <defs/standard.h>

#include "object_lists.h"

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
	for (const auto& [nid, pc] : player_clients)
		if (const auto player = pc->get_player(); player->get_character() == character)
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

void ObjectLists::clear_object_list()
{
	// erase Player instances in this list because PlayerClient 
	// destroys them in the dtor

	std::erase_if(net_objects, [](const auto& p) { return p.second->get_type() == NetObject_Player; });

	// destroys all player clients

	auto it = player_clients.begin();
	
	while (it != player_clients.end())
	{
		DESTROY_PLAYER_CLIENT(it->second);

		it = player_clients.erase(it);
	}

	check(player_clients.empty(), "Player client list must be empty");

	// free all net objects remaining that are not Player

	for (const auto& [_, obj] : net_objects)
		JC_FREE(obj);

	net_objects.clear();
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
		break;
	}
	default:
		check(false, "Invalid net object type");
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
		break;
	}
	default:
		check(false, "Invalid net object type");
	}

	net_objects.erase(nid);

	return true;
}

Player* ObjectLists::get_player_by_nid(NID nid)
{
	const auto pc = get_player_client_by_nid(nid);
	return pc ? pc->get_player() : nullptr;
}

PlayerClient* ObjectLists::get_player_client_by_nid(NID nid)
{
	auto it = player_clients.find(nid);
	return it != player_clients.end() ? it->second : nullptr;
}