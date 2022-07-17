#pragma once

// network id of an object
//
using NID = uint32_t;

static constexpr NID INVALID_NID = 0u;

#ifdef JC_SERVER
namespace enet
{
	void INIT_NIDS_POOL();

	NID GET_FREE_NID();

	void FREE_NID(NID nid);
}
#endif

enum NetObjectType : uint32_t
{
	NetObject_Invalid,
	NetObject_Player,
};

class PlayerClient;
class Player;

class NetObject
{
private:

	PlayerClient* player_client = nullptr;

	Player* streamer = nullptr;

	NID nid = INVALID_NID;

	bool spawned = false;

public:

#ifdef JC_SERVER
	NetObject();
	virtual ~NetObject();
#endif

	virtual uint32_t get_type() const = 0;
	virtual bool spawn() = 0;

#ifdef JC_CLIENT
	void set_nid(NID v) { nid = v; }
#else
	void set_streamer(Player* v) { streamer = v; }
#endif

	void set_spawned(bool v) { spawned = v; }

	void set_player_client(PlayerClient* pc)
	{
		check(get_type() == NetObject_Player, "NetObject must be a player");

		player_client = pc;
	}

	bool is_spawned() const { return spawned; }
	bool equal(NetObject* net_obj) const { return nid == net_obj->nid; }
	bool equal(NID _nid) const { return nid == _nid; }

	NID get_nid() const { return nid; }

	Player* get_streamer() const { return streamer; }

	PlayerClient* get_player_client() const { return player_client; }

	template <typename T>
	T* cast() const
	{
		if (!this)
			return nullptr;

		return (get_type() == T::TYPE() ? BITCAST(T*, this) : nullptr);
	}

	template <typename T>
	T* cast_safe() const
	{
		const auto casted = this->cast<T>();

		check(casted, "Invalid NetObject cast type");

		return casted;
	}
};