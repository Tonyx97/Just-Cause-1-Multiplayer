#pragma once

#include <shared_mp/objs/net_object.h>

class DamageableNetObject : public NetObject
{
private:

#ifdef JC_CLIENT
	class DamageableObject* obj = nullptr;
#endif

	void destroy_object();

public:

	static constexpr NetObjectType TYPE() { return NetObject_Damageable; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	DamageableNetObject(NID nid, const TransformTR& transform);

	class ObjectBase* get_object_base() const override;
#else
	DamageableNetObject(SyncType sync_type, const TransformTR& transform);
#endif
	~DamageableNetObject();

	void on_spawn() override;
	void on_despawn() override;
	void on_sync() override;
	void on_net_var_change(NetObjectVarType var_type) override;
	void serialize_derived(const Packet* p) override {}
	void deserialize_derived(const Packet* p) override {}
};

#define CREATE_DAMAGEABLE_NET_OBJECT(...)	JC_ALLOC(DamageableNetObject, __VA_ARGS__)