#pragma once

#ifndef JC_LOADER
#include <shared_mp/objs/net_object.h>

#include <net/interface.h>

class ObjectLists
{
private:
	std::unordered_map<NID, PlayerClient*> player_clients;
	std::unordered_map<NID, Player*> players;
	std::unordered_map<NID, NetObject*> net_objects;

	NetObject* get_net_object_by_nid_impl(NID nid);

public:

#ifdef JC_CLIENT
	PlayerClient* add_player_client(NID nid);
#else
	PlayerClient* add_player_client(ENetPeer* peer);
#endif

	bool remove_player_client(PlayerClient* pc);

	template <typename T = NetObject>
	T* get_net_object_by_nid(NID nid)
	{
		const auto net_obj = get_net_object_by_nid_impl(nid);

		if constexpr (std::is_same_v<T, NetObject>)
			return net_obj;
		else return net_obj->cast<T>();
	}

	template <typename Fn>
	void for_each_net_object(const Fn& fn)
	{
		for (const auto& [nid, obj] : net_objects)
			fn(nid, obj);
	}

	size_t get_player_clients_count() const { return player_clients.size(); }
	size_t get_net_objects_count() const { return net_objects.size(); }

	NetObject* add_net_object(NetObject* net_obj);

	bool remove_net_object(NetObject* net_obj);

	Player* get_player_by_nid(NID nid);
	PlayerClient* get_player_client_by_nid(NID nid);
};
#endif