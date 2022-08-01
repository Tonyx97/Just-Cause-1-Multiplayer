#include <defs/standard.h>

#include "object_lists.h"

#include <shared_mp/objs/all.h>

#include <mp/net.h>

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

NetObject* ObjectLists::get_net_object_by_game_object(ObjectBase* obj_base)
{
	for (const auto& [nid, net_object] : net_objects)
		if (net_object->get_object_base() == obj_base)
			return net_object;

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
		player_clients.insert({ nid, BITCAST(Player*, net_obj)->get_client()});
		break;
	}
	case NetObject_Damageable:
	{
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

// spawning

NetObject* ObjectLists::spawn_net_object(
#ifdef JC_CLIENT
	NID nid, 
#endif
	NetObjectType type,
	const TransformTR& transform)
{
	NetObject* object = nullptr;

	switch (type)
	{
#ifdef JC_CLIENT
	case NetObject_Damageable: object = CREATE_DAMAGEABLE_NET_OBJECT(nid, transform); break;
#else
	case NetObject_Damageable: object = CREATE_DAMAGEABLE_NET_OBJECT(transform); break;
#endif
	default: log(RED, "Invalid net object type to spawn: {}", type);
	}

	if (object)
	{
		check(add_net_object(object), "Could not add a damageable");

		object->spawn();

#ifdef JC_SERVER
		g_net->send_broadcast_joined_reliable<ChannelID_World>(WorldPID_SpawnObject, object, transform);
#endif
	}

	return object;
}