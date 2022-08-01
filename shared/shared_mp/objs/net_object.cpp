#include <defs/standard.h>

#include "net_object.h"

#include <shared_mp/objs/player.h>

#ifdef JC_CLIENT
#include <game/object/alive_object/alive_object.h>

#include <mp/net.h>

bool NetObject::sync()
{
	if (!is_owned())
		return false;

	// base sync

	// todojc - check for actual alive objects instances

	TransformTR real_transform;

	const auto object_base = BITCAST(AliveObject*, get_object_base());

	real_transform.t = object_base->get_position();
	real_transform.r = object_base->get_rotation();

	const float real_hp = object_base->get_hp(),
				real_max_hp = object_base->get_max_hp();

	if (real_transform.t != get_position() || real_transform.r != get_rotation())
		g_net->send_unreliable<ChannelID_World>(WorldPID_SyncObject, this, NetObjectVar_Transform, (vars.transform = real_transform).pack());

	if (real_hp != get_hp())
		g_net->send_reliable<ChannelID_World>(WorldPID_SyncObject, this, NetObjectVar_Health, vars.hp = real_hp);

	if (real_max_hp != get_max_hp())
		g_net->send_reliable<ChannelID_World>(WorldPID_SyncObject, this, NetObjectVar_MaxHealth, vars.max_hp = real_max_hp);

	// parent object sync

	on_sync();

	return true;
}

bool NetObject::is_owned() const
{
	if (const auto player = cast<Player>())
		return player->is_local();
	else return streamer && streamer->is_local();
}
#else
namespace enet
{
	std::unordered_set<NID> free_nids,
							used_nids;

	void INIT_NIDS_POOL()
	{
		// create a total of 2048 possible nids (1-2048)

		for (NID i = 1u; i < 2048u + 1u; ++i)
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

NetObject::NetObject()
{
	nid = enet::GET_FREE_NID();
}
#endif

NetObject::~NetObject()
{
#ifdef JC_SERVER
	enet::FREE_NID(nid);
#endif
}

void NetObject::set_streamer(Player* v)
{
#ifdef JC_SERVER
	// if the object currently has a streamer and we want to set a new valid streamer
	// then we will transfer the ownership to the new streamer. On the contrary, if this object
	// has no streamer but we want to set a new one, we will simply set the ownership to the new
	// streamer

	if (streamer && v)
		streamer->transfer_net_object_ownership_to(this, v);
	else if (!streamer && v)
		v->set_net_object_ownership_of(this);
#endif

	streamer = v;
}

void NetObject::set_transform(const TransformTR& transform)
{
	vars.transform.t = transform.t;
	vars.transform.r = transform.r;

	if (spawned)
		on_net_var_change(NetObjectVar_Transform);
}

void NetObject::set_transform(const TransformPackedTR& packed_transform)
{
	set_transform(TransformTR(packed_transform));
}

void NetObject::set_position(const vec3& v)
{
	vars.transform.t = v;

	if (spawned)
		on_net_var_change(NetObjectVar_Position);
}

void NetObject::set_rotation(const quat& v)
{
	vars.transform.r = v;

	if (spawned)
		on_net_var_change(NetObjectVar_Rotation);
}

void NetObject::set_hp(float v)
{
	vars.hp = v;

	if (spawned)
		on_net_var_change(NetObjectVar_Health);
}

void NetObject::set_max_hp(float v)
{
	vars.max_hp = v;

	if (spawned)
		on_net_var_change(NetObjectVar_MaxHealth);
}