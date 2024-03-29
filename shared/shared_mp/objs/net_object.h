#pragma once

#include <game/transform/transform.h>

#include <timer/timer.h>

class PlayerClient;

// network id of an object
//
using NID = uint16_t;

static constexpr NID INVALID_NID = 0ui16;

#ifdef JC_CLIENT
#define IF_CLIENT_AND_VALID_OBJ_DO(fn)			if (get_object_base()) \
													fn(); \

#define IF_CLIENT_AND_VALID_CHARACTER_DO(fn)	verify_exec(fn); \

#define SYNC_PARAM							, bool do_sync
#define SYNC_PARAM_DECL						, bool do_sync = false
#define SYNC_IF_TRUE(...)					if (do_sync) { g_net->send(Packet(__VA_ARGS__)); }
#else
#define IF_CLIENT_AND_VALID_OBJ_DO(fn)			

#define IF_CLIENT_AND_VALID_CHARACTER_DO(fn)	

#define SYNC_PARAM							, bool do_sync, PlayerClient* ignore_pc
#define SYNC_PARAM_DECL						, bool do_sync = false, PlayerClient* ignore_pc = nullptr
#define SYNC_IF_TRUE(...)					if (do_sync) { g_net->send_broadcast_joined(ignore_pc, Packet(__VA_ARGS__)); }
#endif

#ifdef JC_CLIENT
namespace net_object_globals
{
	void clear_owned_entities();
}
#else
namespace enet
{
	void INIT_NIDS_POOL();

	NID GET_FREE_NID();

	void FREE_NID(NID nid);
}
#endif

DEFINE_ENUM(NetObjectType, uint8_t)
{
	NetObject_Invalid,
	NetObject_Player,
	NetObject_Damageable,
	NetObject_Blip,
	NetObject_Vehicle,
	NetObject_Pickup,
	NetObject_Grenade,
	NetObject_Max,
};

DEFINE_ENUM(SyncType, uint8_t)
{
	SyncType_None,
	SyncType_Global,			// global sync used by the server to sync stuff such as player blips etc
	SyncType_Distance,			// sync reliable on distance to the closest player
	SyncType_Locked,			// sync locked to a specific player at a given moment
};

DEFINE_ENUM(NetObjectVarType, uint8_t)
{
	NetObjectVar_Begin,
	NetObjectVar_Transform = NetObjectVar_Begin,
	NetObjectVar_Position,
	NetObjectVar_Rotation,
	NetObjectVar_Velocity,
	NetObjectVar_Health,
	NetObjectVar_MaxHealth,
	NetObjectVar_End,
};

DEFINE_ENUM(NetObjectActionSyncType, uint8_t)
{
	NetObjectActionSyncType_Create,
	NetObjectActionSyncType_Hide,
	NetObjectActionSyncType_Destroy,
};

class Player;
class EntityRg;
class Packet;

template <typename T>
struct NetVarInfo
{
	T old {},
	  curr {};

	NetVarInfo() {}
	NetVarInfo(const T& v) { curr = v; }

	void set(const T& v)
	{
		old = curr;
		curr = v;
	}

	const T& get_old() const { return old; }
	const T& get() const { return curr; }
};

class NetObject
{
protected:
	
	NetVarInfo<TransformTR> transform {};

	NetVarInfo<vec3> velocity {},
					 pending_velocity {};

	NetVarInfo<float> hp = jc::nums::MAXF,
					  max_hp = jc::nums::MAXF;

private:

	TimerRaw transform_timer;
	TimerRaw velocity_timer;

	void* userdata = nullptr;

	SyncType sync_type = SyncType_None;

	NID nid = INVALID_NID;

	bool spawned = false;

#ifdef JC_SERVER
	EntityRg* rg = nullptr;
#endif

public:

	static constexpr NetObject* INVALID() { return nullptr; }

	static constexpr float MIN_HP() { return -MAX_HP(); }
	static constexpr float MAX_HP() { return 10000.f * 10000.f; }

	NetObject();

	bool sync();

	virtual ~NetObject() = 0;
	virtual void on_spawn() = 0;
	virtual void on_despawn() = 0;
	virtual void on_sync() = 0;
	virtual void serialize_derived(const Packet* p) = 0;
	virtual void deserialize_derived(const Packet* p) = 0;
	virtual void serialize_derived_create(const Packet* p) = 0;
	virtual void deserialize_derived_create(const Packet* p) = 0;

#ifdef JC_CLIENT
	virtual class ObjectBase* get_object_base() const = 0;
#endif

	virtual void on_net_var_change(NetObjectVarType var_type) = 0;

	bool spawn();
	bool despawn();

	virtual NetObjectType get_type() const = 0;

#ifdef JC_CLIENT
	void set_nid(NID v) { nid = v; }
	void set_transform_timer(int64_t v);
	void set_velocity_timer(int64_t v);
	void set_as_owned();

	bool is_owned();
#else
	void set_sync_type(SyncType v) { sync_type = v; }
	void set_owner(Player* new_owner);
	void set_sync_type_and_owner(SyncType _sync_type, Player* _owner);

	// the following methods are used by the server to force sync with other players
	// and send them the latest info

	void sync_transform(bool reliable = false, PlayerClient* ignore_pc = nullptr);
	void sync_hp(bool reliable = true, PlayerClient* ignore_pc = nullptr);
	void sync_max_hp(bool reliable = true, PlayerClient* ignore_pc = nullptr);
	void sync_velocity(bool reliable = false, PlayerClient* ignore_pc = nullptr);

	bool is_owned_by(Player* player) const;

	EntityRg* get_rg() const { return rg; }
#endif

	// shared functions to set the object vars
	
	void set_transform(const TransformTR& v);
	void set_transform(const TransformPackedTR& v);
	void set_position(const vec3& v);
	void set_rotation(const quat& v);
	void set_hp(float v, bool force_set = false);
	void set_max_hp(float v);
	void set_velocity(const vec3& v);
	void set_pending_velocity(const vec3& v);

	// net object general functions
	
	template <typename T>
	void set_userdata(const T& v)
	{
		if constexpr (std::is_pointer_v<T>)
			userdata = v;
		else userdata = &v;
	}

	void set_spawned(bool v);

	bool is_valid_type() const;
	bool is_spawned() const;
	bool equal(NetObject* net_obj) const { return nid == net_obj->nid; }
	bool equal(NID _nid) const { return nid == _nid; }
	bool was_just_killed() const { return hp.get_old() > 0.f && hp.get() <= 0.f; }
	bool was_just_revived() const { return hp.get_old() <= 0.f && hp.get() > 0.f; }
	bool is_alive() const { return get_hp() > 0.f; }

	NID get_nid() const { return nid; }

	SyncType get_sync_type() const { return sync_type; }

	template <typename T>
	T* get_userdata() const { return BITCAST(T*, userdata); }

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

	float get_hp() const { return hp.curr; }
	float get_max_hp() const { return max_hp.curr; }

	const vec3& get_velocity() const { return velocity.curr; }
	const vec3& get_position() const { return transform.curr.t; }
	const quat& get_rotation() const { return transform.curr.r; }

	const TransformTR& get_transform() const { return transform.curr; }
};

// nice macros to use while defining the scripting functions to avoid
// code duplication

#ifdef JC_CLIENT
#define SYNC_NET_VAR(obj, name, reliable)
#else
#define SYNC_NET_VAR(obj, name, reliable) obj->sync_##name(reliable);
#endif