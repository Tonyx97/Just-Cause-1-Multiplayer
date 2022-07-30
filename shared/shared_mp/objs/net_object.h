#pragma once

// network id of an object
//
using NID = uint32_t;

using NetObjectType = uint8_t;
using SyncType = uint8_t;

static constexpr NID INVALID_NID = 0u;

#ifdef JC_SERVER
namespace enet
{
	void INIT_NIDS_POOL();

	NID GET_FREE_NID();

	void FREE_NID(NID nid);
}
#endif

enum _NetObjectType : NetObjectType
{
	NetObject_Invalid,
	NetObject_Player,
	NetObject_Damageable,
};

enum _SyncType : SyncType
{
	SyncType_None,
	SyncType_Global,			// global sync used by the server to sync stuff such as player blips etc
	SyncType_Distance,			// sync reliable on distance to the closest player
	SyncType_Locked,			// sync locked to a specific player at a given moment
};

class Player;

class NetObject
{
private:

	Player* streamer = nullptr;

	SyncType sync_type = SyncType_None;

	NID nid = INVALID_NID;

	bool spawned = false;

public:

#ifdef JC_SERVER
	NetObject();
#endif

	virtual ~NetObject() = 0;

	virtual bool spawn() = 0;

	virtual NetObjectType get_type() const = 0;

#ifdef JC_CLIENT
	void set_nid(NID v) { nid = v; }
#endif

	void set_sync_type(SyncType v) { sync_type = v; }
	void set_streamer(Player* v) { streamer = v; }
	void set_spawned(bool v) { spawned = v; }

	bool is_spawned() const { return spawned; }
	bool equal(NetObject* net_obj) const { return nid == net_obj->nid; }
	bool equal(NID _nid) const { return nid == _nid; }

	NID get_nid() const { return nid; }

	SyncType get_sync_type() const { return sync_type; }

	Player* get_streamer() const { return streamer; }

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

	virtual vec3 get_position() const = 0;
	virtual quat get_rotation() const = 0;
};