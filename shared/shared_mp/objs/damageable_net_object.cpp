#include <defs/standard.h>

#include <mp/net.h>

#include "damageable_net_object.h"

#ifdef JC_CLIENT
#include <game/sys/factory_system.h>

DamageableNetObject::DamageableNetObject(NID nid, const vec3& position)
{
	set_nid(nid);

	this->position = position;
}
#else
DamageableNetObject::DamageableNetObject(const vec3& position)
{
	set_sync_type(SyncType_Distance);

	this->position = position;
}
#endif

DamageableNetObject::~DamageableNetObject()
{
}

bool DamageableNetObject::spawn()
{
	// if it's already spawned then do nothing

	if (is_spawned())
	{
		log(RED, "DamageableObject {:x} was already spawned, where are you calling this from?", get_nid());

		return false;
	}

#ifdef JC_CLIENT
	obj = g_factory->spawn_damageable_object(position, "building_blocks\\general\\oil_barrel_red.lod", "models\\building_blocks\\general\\oil_barrel.pfx");

	check(obj, "Could not create damageable object");

	log(PURPLE, "DamageableObject {:x} spawned now {:x}", get_nid(), ptr(obj));
#endif

	set_spawned(true);

	return true;
}