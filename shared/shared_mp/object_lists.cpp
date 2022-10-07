#include <defs/standard.h>

#include "object_lists.h"

#include <shared_mp/player_client/player_client.h>
#include <shared_mp/objs/all.h>

#include <mp/net.h>

void ObjectLists::for_each_net_object(const nid_net_obj_fn_t& fn)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	for (const auto& [nid, obj] : net_objects)
		fn(nid, obj);
}

void ObjectLists::for_each_player_client(const nid_pc_fn_t& fn)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	for (const auto& [nid, pc] : player_clients)
		fn(nid, pc);
}

void ObjectLists::for_each_joined_player_client(const nid_pc_fn_t& fn)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	for (const auto& [nid, pc] : player_clients)
		if (pc->is_joined())
			fn(nid, pc);
}

void ObjectLists::for_each_player(const nid_player_fn_t& fn)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	for (const auto& [nid, player] : player_clients)
		fn(nid, player->get_player());
}

#ifdef JC_CLIENT
PlayerClient* ObjectLists::add_player_client(NID nid)
{
	const auto pc = CREATE_PLAYER_CLIENT(nid);

	check(pc, "Could not create PlayerClient");
	check(add_net_object(pc->get_player()), "Could not add a player client");

	return pc;
}

Player* ObjectLists::get_player_by_character(Character* character)
{
	if (!character)
		return nullptr;

	for (const auto& [nid, pc] : player_clients)
		if (const auto player = pc->get_player(); player->get_character() == character)
			return player;

	return nullptr;
}

NetObject* ObjectLists::get_net_object_by_game_object(ObjectBase* obj_base)
{
	if (!obj_base)
		return nullptr;

	for (const auto& [nid, net_object] : net_objects)
		if (net_object->get_object_base() == obj_base)
			return net_object;

	return nullptr;
}
#else
PlayerClient* ObjectLists::add_player_client(ENetPeer* peer)
{
	const auto pc = CREATE_PLAYER_CLIENT(peer);

	check(add_net_object(pc->get_player()), "Could not add a player client");

	peer->data = pc;

	return pc;
}
#endif

bool ObjectLists::remove_player_client(PlayerClient* pc)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	check(pc, "Invalid player client at '{}'", CURR_FN);

	remove_net_object(pc->get_player());

	DESTROY_PLAYER_CLIENT(pc);

	return true;
}

bool ObjectLists::has_player_client(PlayerClient* pc) const
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	return player_clients_set.contains(pc);
}

void ObjectLists::clear_object_list()
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	// erase Player instances in this list because PlayerClient 
	// destroys them in the dtor

	std::erase_if(net_objects, [](const auto& p) { return p.second->get_type() == NetObject_Player; });

	// destroys all player clients

	auto it = player_clients.begin();
	
	while (it != player_clients.end())
	{
		const auto pc = it->second;

		DESTROY_PLAYER_CLIENT(pc);

		player_clients_set.erase(pc);

		it = player_clients.erase(it);
	}

	check(player_clients.empty(), "Player client list must be empty");
	check(player_clients_set.empty(), "Player client set must be empty");

	// free all net objects remaining that are not Player

	for (const auto& [_, obj] : net_objects)
		JC_FREE(obj);

	net_objects.clear();
}

NetObject* ObjectLists::get_net_object_by_nid_impl(NID nid)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	auto it = net_objects.find(nid);
	return it != net_objects.end() ? it->second : nullptr;
}

NetObject* ObjectLists::add_net_object(NetObject* net_obj)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	const auto nid = net_obj->get_nid();

	switch (const auto type = net_obj->get_type())
	{
	case NetObject_Player:
	{
		const auto pc = BITCAST(Player*, net_obj)->get_client();

		player_clients.insert({ nid, pc });
		player_clients_set.insert(pc);

		break;
	}
	default:
	{
		// if the object has no valid type then it's an invalid net object

		if (!net_obj->is_valid_type())
		{
			check(false, "Invalid net object type");

			return nullptr;
		}
	}
	}

	net_objects.insert({ nid, net_obj });

	return net_obj;
}

bool ObjectLists::remove_net_object(NetObject* net_obj)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	const auto nid = net_obj->get_nid();

	switch (net_obj->get_type())
	{
	case NetObject_Player:
	{
		const auto player = net_obj->cast_safe<Player>();

		player_clients.erase(nid);
		player_clients_set.erase(player->get_client());

		break;
	}
	default:
	{
		// if the object has no valid type then it's an invalid net object
		
		if (!net_obj->is_valid_type())
		{
			check(false, "Invalid net object type");

			return false;
		}
	}
	}

	net_objects.erase(nid);

	return true;
}

Player* ObjectLists::get_player_by_nid(NID nid)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	const auto pc = get_player_client_by_nid(nid);
	return pc ? pc->get_player() : nullptr;
}

PlayerClient* ObjectLists::get_player_client_by_nid(NID nid)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	auto it = player_clients.find(nid);
	return it != player_clients.end() ? it->second : nullptr;
}

PlayerClient* ObjectLists::get_valid_player_client(PlayerClient* pc)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	auto it = player_clients_set.find(pc);
	return it != player_clients_set.end() ? *it : nullptr;
}

// spawning

bool ObjectLists::destroy_net_object(NetObject* obj)
{
	check(!obj->cast<Player>(), "Cannot destroy a Player using this function, use ::remove_player_client");

#ifdef JC_SERVER
	g_net->send_broadcast_joined(Packet(WorldPID_DestroyObject, ChannelID_World, obj));
#endif

	if (!remove_net_object(obj))
		return false;

	JC_FREE(obj);

	return true;
}

NetObject* ObjectLists::spawn_net_object(
#ifdef JC_CLIENT
	NID nid,
#else
	SyncType sync_type,
#endif
	NetObjectType type,
	uint16_t object_id,
	const TransformTR& transform)
{
	NetObject* object = nullptr;

	switch (type)
	{
#ifdef JC_CLIENT
	case NetObject_Damageable: object = CREATE_DAMAGEABLE_NET_OBJECT(nid, transform); break;
	case NetObject_Blip: object = CREATE_BLIP_NET_OBJECT(nid, transform); break;
	case NetObject_Vehicle: object = CREATE_VEHICLE_NET_OBJECT(nid, transform); break;
#else
	case NetObject_Damageable: object = CREATE_DAMAGEABLE_NET_OBJECT(sync_type, transform); break;
	case NetObject_Blip: object = CREATE_BLIP_NET_OBJECT(sync_type, transform); break;
	case NetObject_Vehicle: object = CREATE_VEHICLE_NET_OBJECT(sync_type, transform); break;
#endif
	default: log(RED, "Invalid net object type to spawn: {}", type);
	}

	check(object, "Could not create a net object");

	if (object)
	{
		check(add_net_object(object), "Could not add a net object");

		object->set_object_id(object_id);
		object->spawn();

#ifdef JC_SERVER
		g_net->sync_net_objects(true);
#endif
	}

	return object;
}