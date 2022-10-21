#include <defs/standard.h>

#include "object_lists.h"

#include <shared_mp/player_client/player_client.h>
#include <shared_mp/objs/all.h>

#include <resource_sys/resource_system.h>

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

	for (const auto& [nid, pc] : player_clients)
		if (!fn(nid, pc->get_player()))
			return;
}

void ObjectLists::for_each_joined_player(const nid_player_fn_t& fn)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	for (const auto& [nid, pc] : player_clients)
		if (pc->is_joined())
			if (!fn(nid, pc->get_player()))
				return;
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

	check(pc, "Invalid player client in '{}'", CURR_FN);

	const auto player = pc->get_player();

	g_rsrc->trigger_event(script::event::ON_PLAYER_QUIT, player);

	remove_net_object(player);

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

bool ObjectLists::has_net_object(NetObject* net_obj) const
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	return net_objects_set.contains(net_obj);
}

void ObjectLists::clear_object_list()
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	// erase Player instances in net_objects and net_objects_set list because PlayerClient 
	// destroys them in the dtor

	std::erase_if(net_objects, [this](const auto& p)
	{
		const auto net_obj = p.second;

		if (net_obj->get_type() == NetObject_Player)
		{
			net_objects_set.erase(net_obj);

			return true;
		}

		return false;
	});

	// destroys all player clients

	auto it = player_clients.begin();
	
	while (it != player_clients.end())
	{
		const auto pc = it->second;

		net_objects_set.erase(pc->get_player());
		player_clients_set.erase(pc);

		DESTROY_PLAYER_CLIENT(pc);

		it = player_clients.erase(it);
	}

	check(player_clients.empty(), "Player client list must be empty");
	check(player_clients_set.empty(), "Player client set must be empty");

	// free all net objects remaining that are not Player

	for (const auto& [_, obj] : net_objects)
		JC_FREE(obj);

	net_objects_set.clear();
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
	if (!net_obj)
		return nullptr;

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
	net_objects_set.insert(net_obj);

	return net_obj;
}

NetObject* ObjectLists::get_net_object(NetObject* net_obj)
{
	return has_net_object(net_obj) ? net_obj : nullptr;
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
	net_objects_set.erase(net_obj);

	return true;
}

Player* ObjectLists::get_player(NetObject* net_obj)
{
	return (has_net_object(net_obj) && net_obj->get_type() == NetObject_Player) ? net_obj->cast<Player>() : nullptr;
}

Player* ObjectLists::get_player_by_nid(NID nid)
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	const auto pc = get_player_client_by_nid(nid);
	return pc ? pc->get_player() : nullptr;
}

Player* ObjectLists::get_random_player()
{
#ifdef JC_SERVER
	std::lock_guard lock(mtx);
#endif

	if (player_clients.empty())
		return nullptr;

	std::vector<Player*> joined_players,
						 out;

	for_each_joined_player_client([&](NID, PlayerClient* pc)
	{
		joined_players.push_back(pc->get_player());
	});

	std::sample(joined_players.begin(), joined_players.end(), std::back_inserter(out), 1, std::mt19937_64(std::random_device {} ()));

	return out.empty() ? nullptr : *out.begin();
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

#ifdef JC_SERVER
	g_net->sync_net_objects(true);
#endif

	return true;
}

NetObject* ObjectLists::spawn_net_object(
#ifdef JC_CLIENT
	NID nid,
#else
	SyncType sync_type,
#endif
	NetObjectType type,
	const TransformTR& transform,
	const Packet* p)
{
	NetObject* object = nullptr;

	switch (type)
	{
#ifdef JC_CLIENT
	case NetObject_Damageable:	object = CREATE_DAMAGEABLE_NET_OBJECT(nid, transform); break;
	case NetObject_Blip:		object = CREATE_BLIP_NET_OBJECT(nid, transform); break;
	case NetObject_Vehicle:		object = CREATE_VEHICLE_NET_OBJECT(nid, transform); break;
	case NetObject_Pickup:		object = CREATE_PICKUP_NET_OBJECT(nid, transform); break;
#else
	case NetObject_Damageable:	object = CREATE_DAMAGEABLE_NET_OBJECT(sync_type, transform); break;
	case NetObject_Blip:		object = CREATE_BLIP_NET_OBJECT(sync_type, transform); break;
	case NetObject_Vehicle:		object = CREATE_VEHICLE_NET_OBJECT(sync_type, transform); break;
	case NetObject_Pickup:		object = CREATE_PICKUP_NET_OBJECT(sync_type, transform); break;
#endif
	default: log(RED, "Invalid net object type to spawn: {}", type);
	}

	check(add_net_object(object), "Could not create a net object");

	// deserialize from packet the basic info required to create the object
		
	object->deserialize_derived_create(p);

	// spawn the object
		
	object->spawn();

#ifdef JC_SERVER
	g_net->sync_net_objects(true);
#endif

	return object;
}

NetObject* ObjectLists::spawn_damageable(
#ifdef JC_CLIENT
	NID nid,
#else
	SyncType sync_type,
#endif
	NetObjectType type,
	const TransformTR& transform,
	const std::string& lod,
	const std::string& pfx)
{
#ifdef JC_CLIENT
	const auto object = CREATE_DAMAGEABLE_NET_OBJECT(nid, transform);
#else
	const auto object = CREATE_DAMAGEABLE_NET_OBJECT(sync_type, transform);
#endif

	check(add_net_object(object), "Could not create a damageable");

	object->set_lod(lod);
	object->set_pfx(pfx);
	object->spawn();

#ifdef JC_SERVER
	g_net->sync_net_objects(true);
#endif

	return object;
}

NetObject* ObjectLists::spawn_blip(
#ifdef JC_CLIENT
	NID nid,
#else
	SyncType sync_type,
#endif
	NetObjectType type,
	const TransformTR& transform)
{
#ifdef JC_CLIENT
	const auto object = CREATE_BLIP_NET_OBJECT(nid, transform);
#else
	const auto object = CREATE_BLIP_NET_OBJECT(sync_type, transform);
#endif

	check(add_net_object(object), "Could create a blip");

	object->spawn();

#ifdef JC_SERVER
	g_net->sync_net_objects(true);
#endif

	return object;
}

NetObject* ObjectLists::spawn_vehicle(
#ifdef JC_CLIENT
	NID nid,
#else
	SyncType sync_type,
#endif
	NetObjectType type,
	const TransformTR& transform,
	const std::string& ee_name)
{
#ifdef JC_CLIENT
	const auto object = CREATE_VEHICLE_NET_OBJECT(nid, transform);
#else
	const auto object = CREATE_VEHICLE_NET_OBJECT(sync_type, transform);
#endif

	check(add_net_object(object), "Could not create a vehicle");

	object->set_ee(ee_name);
	object->spawn();

#ifdef JC_SERVER
	g_net->sync_net_objects(true);
#endif

	return object;
}

NetObject* ObjectLists::spawn_pickup(
#ifdef JC_CLIENT
	NID nid,
#else
	SyncType sync_type,
#endif
	NetObjectType type,
	const TransformTR& transform,
	uint32_t pickup_type,
	const std::string& lod)
{
#ifdef JC_CLIENT
	const auto object = CREATE_PICKUP_NET_OBJECT(nid, transform);
#else
	const auto object = CREATE_PICKUP_NET_OBJECT(sync_type, transform);
#endif

	check(add_net_object(object), "Could not create a pickup");

	object->set_item_type(pickup_type);
	object->set_lod(lod);
	object->spawn();

#ifdef JC_SERVER
	g_net->sync_net_objects(true);
#endif

	return object;
}
