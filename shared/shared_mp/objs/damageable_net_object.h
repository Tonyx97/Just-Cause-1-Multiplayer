#pragma once

#include <shared_mp/objs/net_object.h>

class DamageableNetObject : public NetObject
{
private:

	std::string lod,
				pfx;

#ifdef JC_CLIENT
	shared_ptr<class DamageableObject> obj;
#endif

	void destroy_object();

public:

	static constexpr NetObjectType TYPE() { return NetObject_Damageable; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	DamageableNetObject(NID nid, const TransformTR& transform);

	class DamageableObject* get_object() const;

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
	void serialize_derived_create(const Packet* p) override;
	void deserialize_derived_create(const Packet* p) override;

	void set_lod(const std::string& v) { lod = v; }
	void set_pfx(const std::string& v) { pfx = v; }

	const std::string& get_lod() const { return lod; }
	const std::string& get_pfx() const { return pfx; }
};

#define CREATE_DAMAGEABLE_NET_OBJECT(...)	JC_ALLOC(DamageableNetObject, __VA_ARGS__)