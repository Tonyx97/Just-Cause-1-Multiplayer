#pragma once

#include <shared_mp/objs/net_object.h>

class DamageableNetObject : public NetObject
{
private:

#ifdef JC_CLIENT
	class DamageableObject* obj = nullptr;
#endif

public:

	static constexpr NetObjectType TYPE() { return NetObject_Damageable; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	DamageableNetObject(NID nid, const TransformTR& transform);

	void on_sync() override;

	class ObjectBase* get_object_base() override;
#else
	DamageableNetObject(const TransformTR& transform);
#endif
	~DamageableNetObject();

	void on_net_var_change(NetObjectVarType var_type) override;

	bool spawn() override;
};

#define CREATE_DAMAGEABLE_NET_OBJECT(...)	JC_ALLOC(DamageableNetObject, __VA_ARGS__)