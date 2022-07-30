#pragma once

#include <shared_mp/objs/net_object.h>

#include <game/transform/transform.h>

#ifdef JC_CLIENT
class DamageableObject;
#endif

class DamageableNetObject : public NetObject
{
private:

#ifdef JC_CLIENT
	DamageableObject* obj = nullptr;
#endif

public:

	vec3 position;

	quat rotation;

	static constexpr NetObjectType TYPE() { return NetObject_Damageable; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	DamageableNetObject(NID nid, const vec3& position);
#else
	DamageableNetObject(const vec3& position);
#endif
	~DamageableNetObject();

	bool spawn() override;

	vec3 get_position() const override { return position; }
	quat get_rotation() const override { return rotation; }
};

#define CREATE_DAMAGEABLE_NET_OBJECT(...)	JC_ALLOC(DamageableNetObject, __VA_ARGS__)