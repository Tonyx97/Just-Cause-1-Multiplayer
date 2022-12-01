#include <defs/standard.h>

#include "net_object.h"

#include <shared_mp/objs/player.h>

#ifdef JC_CLIENT
#include <game/object/damageable_object/damageable_object.h>
#include <game/object/ui/map_icon.h>
#include <game/sys/core/factory_system.h>
#else
#include <rg/rg.h>
#endif

#include <mp/net.h>

NetObject::NetObject()
{
#ifdef JC_CLIENT
	transform_timer(16 * 3);
	velocity_timer(0);
#else
	nid = enet::GET_FREE_NID();
	rg = g_net->get_rg()->add_entity(this, static_cast<int64_t>(nid));
#endif
}

NetObject::~NetObject()
{
#ifdef JC_CLIENT
#else
	g_net->get_rg()->remove_entity(rg);

	enet::FREE_NID(nid);
#endif
}

#ifdef JC_CLIENT
namespace net_object_globals
{
	std::unordered_set<NetObject*> owned_entities;

	void clear_owned_entities()
	{
		owned_entities.clear();
	}
}

bool NetObject::sync()
{
	if (!is_owned())
		return false;

	// base sync

	TransformTR real_transform;

	const auto object_base = BITCAST(AliveObject*, get_object_base());
	const auto physical = object_base->get_pfx();

	real_transform.t = object_base->get_position();
	real_transform.r = object_base->get_rotation();

	const float real_hp = object_base->get_real_hp(),
				real_max_hp = object_base->get_max_hp();

	if ((glm::distance2(real_transform.t, get_position()) > 0.00025f || real_transform.r != get_rotation()) && transform_timer.ready())
		g_net->send(Packet(WorldPID_SyncObject, ChannelID_World, this, NetObjectVar_Transform, (transform.curr = real_transform).pack()).set_unreliable());

	switch (get_type())
	{
	case NetObject_Player: break;
	default:
	{
		if (velocity_timer.get_interval() > 0 && velocity_timer.ready())
		{
			const auto curr_velocity = physical->get_velocity();

			if (glm::distance2(curr_velocity, get_velocity()) > 0.0025f)
				g_net->send(Packet(WorldPID_SyncObject, ChannelID_World, this, NetObjectVar_Velocity, velocity.curr = curr_velocity));
		}
	}
	}

	const auto _hp = get_hp(),
			   _max_hp = get_max_hp();

	if (real_hp != _hp)
		g_net->send(Packet(WorldPID_SyncObject, ChannelID_World, this, NetObjectVar_Health, hp.curr = real_hp));

	if (real_max_hp != _max_hp)
		g_net->send(Packet(WorldPID_SyncObject, ChannelID_World, this, NetObjectVar_MaxHealth, max_hp.curr = real_max_hp));

	// parent object sync

	on_sync();

	return true;
}

void NetObject::set_transform_timer(int64_t v)
{
	transform_timer(v);
}

void NetObject::set_velocity_timer(int64_t v)
{
	velocity_timer(v);
}

void NetObject::set_as_owned()
{
	net_object_globals::owned_entities.insert(this);
}

bool NetObject::is_owned()
{
	return net_object_globals::owned_entities.contains(this);
}

bool NetObject::is_spawned() const
{
	return spawned && !!get_object_base();
}
#else
namespace enet
{
	std::unordered_set<NID> free_nids,
							used_nids;

	void INIT_NIDS_POOL()
	{
		// create a total of 65534 possible nids (1-65534)

		for (NID i = 1ui16; i < 65535ui16; ++i)
			free_nids.insert(free_nids.end(), i);
	}

	NID GET_FREE_NID()
	{
		check(!free_nids.empty(), "No more free NIDs available");

		const auto it = free_nids.begin();
		const auto nid = *it;

		free_nids.erase(it);
		used_nids.insert(nid);

		return nid;
	}

	void FREE_NID(NID nid)
	{
		auto it = used_nids.find(nid);

		check(it != used_nids.end(), "NID {} not used", nid);

		used_nids.erase(it);
		free_nids.insert(nid);
	}
}

void NetObject::set_owner(Player* new_owner)
{
	rg->set_owner(new_owner ? new_owner->get_rg() : nullptr);
}

void NetObject::set_sync_type_and_owner(SyncType _sync_type, Player* _owner)
{
	set_sync_type(_sync_type);
	set_owner(_owner);
}

bool NetObject::sync()
{
	log(RED, "server wants to sync this object");

	on_sync();
	
	return true;
}

bool NetObject::is_owned_by(Player* player) const
{
	return rg->get_owner() == player->get_rg();
}

bool NetObject::is_spawned() const
{
	return spawned;
}

void NetObject::sync_transform(bool reliable, PlayerClient* ignore_pc)
{
	g_net->send_broadcast_joined(ignore_pc, Packet(WorldPID_SyncObject, ChannelID_World, this, NetObjectVar_Transform, transform.get().pack()).set_reliable(reliable));
}

void NetObject::sync_hp(bool reliable, PlayerClient* ignore_pc)
{
	g_net->send_broadcast_joined(ignore_pc, Packet(WorldPID_SyncObject, ChannelID_World, this, NetObjectVar_Health, hp.get()).set_reliable(reliable));
}

void NetObject::sync_max_hp(bool reliable, PlayerClient* ignore_pc)
{
	g_net->send_broadcast_joined(ignore_pc, Packet(WorldPID_SyncObject, ChannelID_World, this, NetObjectVar_MaxHealth, max_hp.get()).set_reliable(reliable));
}

void NetObject::sync_velocity(bool reliable, PlayerClient* ignore_pc)
{
	g_net->send_broadcast_joined(ignore_pc, Packet(WorldPID_SyncObject, ChannelID_World, this, NetObjectVar_Velocity, velocity.get()).set_reliable(reliable));
}
#endif

void NetObject::set_transform(const TransformTR& v)
{
	transform.set(v);

	if (spawned)
		on_net_var_change(NetObjectVar_Transform);
}

void NetObject::set_transform(const TransformPackedTR& v)
{
	set_transform(TransformTR(v));
}

void NetObject::set_position(const vec3& v)
{
	transform.set(TransformTR(v, transform.get().r));

	if (spawned)
		on_net_var_change(NetObjectVar_Position);
}

void NetObject::set_rotation(const quat& v)
{
	transform.set(TransformTR(transform.get().t, v));

	if (spawned)
		on_net_var_change(NetObjectVar_Rotation);
}

void NetObject::set_hp(float v, bool force_set)
{
	if (v < MIN_HP() || v > MAX_HP())
		return;

	// if entity is already dead then ignore, we cannot restore the status of a dead entity,
	// we need restore/respawn functions for this but not needed right now

	if (!force_set && !is_alive() && v > 0.f)
		return;

	// ignore if we want to set higher value than the max hp

	if (v > get_max_hp())
		return;

	hp.set(v);

	if (spawned)
		on_net_var_change(NetObjectVar_Health);
}

void NetObject::set_max_hp(float v)
{
	if (v < MIN_HP() || v > MAX_HP())
		return;

	// we cannot set the max hp to less or equal than 0 or the
	// entity would be dead all the time lol

	if (v <= 0.f)
		return;

	// for the sake of avoiding problems in the future, do not change the max health
	// if the entity is dead...

	if (!is_alive())
		return;

	max_hp.set(v);

	if (spawned)
		on_net_var_change(NetObjectVar_MaxHealth);
}

void NetObject::set_velocity(const vec3& v)
{
	velocity.set(v);

	if (spawned)
		on_net_var_change(NetObjectVar_Velocity);
}

void NetObject::set_pending_velocity(const vec3& v)
{
	pending_velocity.set(v);
}

void NetObject::set_spawned(bool v)
{
	spawned = v;

	// if we just spawned the object then let the parent
	// class know so they can set the basic vars

	if (spawned)
		on_net_var_change(NetObjectVar_Transform);
}

bool NetObject::spawn()
{
	// if it's already spawned then do nothing

	if (is_spawned())
	{
		log(RED, "NetObject type {} (NID: {:x}) was already spawned, where are you calling this from?", get_type(), get_nid());

		return false;
	}

	on_spawn();
	set_spawned(true);

	return true;
}

bool NetObject::despawn()
{
	if (!is_spawned())
	{
		log(RED, "NetObject type {} (NID: {:x}) wasn't spawned, where are you calling this from?", get_type(), get_nid());

		return false;
	}

	on_despawn();
	set_spawned(false);

	return true;
}

bool NetObject::is_valid_type() const
{
	const auto type = get_type();

	return type > NetObject_Invalid && type < NetObject_Max;
}