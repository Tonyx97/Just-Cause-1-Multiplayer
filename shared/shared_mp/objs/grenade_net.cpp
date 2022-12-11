#include <defs/standard.h>

#include <mp/net.h>

#include "grenade_net.h"

#ifdef JC_CLIENT
#include <game/sys/core/factory_system.h>

#include <game/object/damageable_object/grenade.h>
#include <game/object/character/character.h>

GrenadeNetObject::GrenadeNetObject(NID nid, const TransformTR& transform)
{
	set_nid(nid);
	set_transform(transform);
	set_transform_timer(500);
	set_velocity_timer(100);
}

PlayerGrenade* GrenadeNetObject::get_object() const
{
	return obj.get();
}

ObjectBase* GrenadeNetObject::get_object_base() const
{
	return get_object();
}
#else
GrenadeNetObject::GrenadeNetObject(SyncType sync_type, const TransformTR& transform)
{
	set_sync_type(sync_type);
	set_transform(transform);
}
#endif

GrenadeNetObject::~GrenadeNetObject()
{
	destroy_object();
}

void GrenadeNetObject::destroy_object()
{
	IF_CLIENT_AND_VALID_OBJ_DO([&]() { obj.reset(); });
}

void GrenadeNetObject::on_sync()
{
}

void GrenadeNetObject::on_spawn()
{
#ifdef JC_CLIENT
	check(owner, "GrenadeNetObject must have an owner");

	const auto owner_char = owner->get_character();

	obj = g_factory->spawn_grenade({});

	check(obj, "Could not create grenade object");

	const auto object = get_object();

	owner_char->set_arms_stance(18);

	jc::this_call(0x747340, object, weak_ptr<ObjectBase>(owner_char->get_shared()));

	jc::write(a2, object, 0x2B0);
	jc::write(a3, object, 0x2BC);

	const auto position = get_position();

	jc::this_call(0x747530, object, &position);

	log(PURPLE, "GrenadeNetObject {:x} spawned now {:x} at {:.2f} {:.2f} {:.2f}", get_nid(), ptr(obj.get()), get_position().x, get_position().y, get_position().z);
#endif
}

void GrenadeNetObject::on_despawn()
{
	destroy_object();
}

void GrenadeNetObject::on_net_var_change(NetObjectVarType var_type)
{
#ifdef JC_CLIENT
	IF_CLIENT_AND_VALID_OBJ_DO([&]()
	{
		switch (var_type)
		{
		case NetObjectVar_Transform:
		case NetObjectVar_Position:
		case NetObjectVar_Rotation: obj->set_transform(Transform(get_position(), get_rotation())); break;
		case NetObjectVar_Velocity: obj->get_pfx()->set_velocity(get_velocity()); break;
		case NetObjectVar_Health: obj->set_hp(get_hp()); break;
		case NetObjectVar_MaxHealth: obj->set_max_hp(get_max_hp()); break;
		}
	});
#else
#endif
}

void GrenadeNetObject::serialize_derived_create(const Packet* p)
{
	p->add(get_owner());
	p->add(get_a2());
	p->add(get_a3());
}

void GrenadeNetObject::deserialize_derived_create(const Packet* p)
{
	set_owner(p->get_net_object<Player>());
	set_a2(p->get<vec3>());
	set_a3(p->get<vec3>());
}