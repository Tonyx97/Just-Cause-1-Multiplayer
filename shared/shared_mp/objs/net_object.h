#pragma once

#include <game/transform/transform.h>

#include <timer/timer.h>

// network id of an object
//
using NID = uint16_t;
using NetObjectType = uint8_t;
using SyncType = uint8_t;
using NetObjectVarType = uint8_t;
using NetObjectActionSyncType = uint8_t;

static constexpr NID INVALID_NID = 0ui16;

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
	NetObject_Blip,
	NetObject_Vehicle,
	NetObject_Max,
};

enum _SyncType : SyncType
{
	SyncType_None,
	SyncType_Global,			// global sync used by the server to sync stuff such as player blips etc
	SyncType_Distance,			// sync reliable on distance to the closest player
	SyncType_Locked,			// sync locked to a specific player at a given moment
};

enum _NetObjectVarType : NetObjectVarType
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

enum _NetObjectActionSyncType : NetObjectActionSyncType
{
	NetObjectActionSyncType_Create,
	NetObjectActionSyncType_Hide,
	NetObjectActionSyncType_Destroy,
};

class Player;
class EntityRg;

struct NetObjectVars
{
	TimerRaw transform_timer;
	TimerRaw velocity_timer;

	TransformTR transform {};

	vec3 velocity {},
		 pending_velocity {};

	float hp = jc::nums::MAXF,
		  max_hp = jc::nums::MAXF;
};

class NetObject
{
private:

	NetObjectVars vars {};

	EntityRg* rg = nullptr;

	void* userdata = nullptr;

	SyncType sync_type = SyncType_None;

	NID nid = INVALID_NID;

	uint16_t object_id = 0ui16;

	bool spawned = false;

#ifdef JC_CLIENT
	bool owned = false;
#endif

public:

	static constexpr NetObject* INVALID() { return nullptr; }

	static constexpr float MIN_HP() { return -MAX_HP(); }
	static constexpr float MAX_HP() { return static_cast<float>(1 << 16); }

	NetObject();

	bool sync();

	virtual ~NetObject() = 0;
	virtual void on_spawn() = 0;
	virtual void on_despawn() = 0;
	virtual void on_sync() = 0;

#ifdef JC_CLIENT
	virtual class ObjectBase* get_object_base() = 0;
#endif

	virtual void on_net_var_change(NetObjectVarType var_type) = 0;

	bool spawn();
	bool despawn();

	virtual NetObjectType get_type() const = 0;

#ifdef JC_CLIENT
	void set_nid(NID v) { nid = v; }
	void set_transform_timer(int64_t v);
	void set_velocity_timer(int64_t v);

	bool is_owned() const;
#endif

	template <typename T>
	void set_userdata(const T& v)
	{
		if constexpr (std::is_pointer_v<T>)
			userdata = v;
		else userdata = &v;
	}

	void set_owner(Player* new_owner);
	void set_spawned(bool v);
	void set_sync_type(SyncType v) { sync_type = v; }
	void set_object_id(uint16_t v) { object_id = v; }
	void set_sync_type_and_owner(SyncType _sync_type, Player* _owner);
	void set_transform(const TransformTR& transform);
	void set_transform(const TransformPackedTR& packed_transform);
	void set_position(const vec3& v);
	void set_rotation(const quat& v);
	void set_hp(float v);
	void set_max_hp(float v);
	void set_velocity(const vec3& v);
	void set_pending_velocity(const vec3& v);

	bool is_valid_type() const;
	bool is_owned_by(Player* player) const;
	bool is_spawned() const { return spawned; }
	bool equal(NetObject* net_obj) const { return nid == net_obj->nid; }
	bool equal(NID _nid) const { return nid == _nid; }

	uint16_t get_object_id() const { return object_id; }

	NID get_nid() const { return nid; }

	SyncType get_sync_type() const { return sync_type; }

	EntityRg* get_rg() const { return rg; }

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

	float get_hp() const { return vars.hp; }
	float get_max_hp() const { return vars.max_hp; }

	const vec3& get_velocity() const { return vars.velocity; }
	const vec3& get_position() const { return vars.transform.t; }
	const quat& get_rotation() const { return vars.transform.r; }

	const TransformTR& get_transform() const { return vars.transform; }
};