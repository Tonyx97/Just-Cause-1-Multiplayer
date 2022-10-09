#pragma once

#include <net/interface.h>

class DamageableNetObject;
class PlayerClient;
class Player;

using nid_net_obj_fn_t = std::function<void(NID, NetObject*)>;
using nid_pc_fn_t = std::function<void(NID, PlayerClient*)>;
using nid_player_fn_t = std::function<void(NID, Player*)>;

class ObjectLists
{
protected:

#ifdef JC_SERVER
	// it's possible that we access the object lists from another thread (TCP connection)
	// so we want to add a recursive mutex: it's recursive because we should be able to 
	// manipulate a player using multiple functions that acquire the lock such as 
	// get_player_client_by_nid
	//
	mutable std::recursive_mutex mtx;
#endif

	void clear_object_list();

private:

	std::unordered_set<PlayerClient*> player_clients_set;
	std::unordered_set<NetObject*> net_objects_set;
	std::unordered_map<NID, PlayerClient*> player_clients;
	std::unordered_map<NID, NetObject*> net_objects;

	NetObject* get_net_object_by_nid_impl(NID nid);

public:

#ifdef JC_CLIENT
	PlayerClient* add_player_client(NID nid);

	Player* get_player_by_character(class Character* character);

	NetObject* get_net_object_by_game_object(class ObjectBase* obj_base);
#else
	PlayerClient* add_player_client(ENetPeer* peer);
#endif

	void for_each_net_object(const nid_net_obj_fn_t& fn);
	void for_each_player_client(const nid_pc_fn_t& fn);
	void for_each_joined_player_client(const nid_pc_fn_t& fn);
	void for_each_player(const nid_player_fn_t& fn);

	bool remove_player_client(PlayerClient* pc);
	bool has_player_client(PlayerClient* pc) const;
	bool has_net_object(NetObject* net_obj) const;

#ifdef JC_SERVER
	template <typename Fn>
	void exec_with_object_lock(const Fn& fn)
	{
		std::lock_guard lock(mtx);

		fn();
	}
#endif

	template <typename T = NetObject>
	T* get_net_object_by_nid(NID nid)
	{
		const auto net_obj = get_net_object_by_nid_impl(nid);

		if constexpr (std::is_same_v<T, NetObject>)
			return net_obj;
		else return net_obj->cast<T>();
	}

	size_t get_player_clients_count() const { return player_clients.size(); }
	size_t get_net_objects_count() const { return net_objects.size(); }

	NetObject* add_net_object(NetObject* net_obj);
	NetObject* get_net_object(NetObject* net_obj);

	bool remove_net_object(NetObject* net_obj);

	Player* get_player(NetObject* net_obj);
	Player* get_player_by_nid(NID nid);
	PlayerClient* get_player_client_by_nid(NID nid);
	PlayerClient* get_valid_player_client(PlayerClient* pc);

	// spawning

	bool destroy_net_object(NetObject* obj);
	
	NetObject* spawn_net_object(
#ifdef JC_CLIENT
		NID nid,
#else
		SyncType sync_type,
#endif
		NetObjectType type,
		uint16_t object_id,
		const TransformTR& transform);
};