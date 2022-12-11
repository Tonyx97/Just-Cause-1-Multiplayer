#pragma once

#include <shared_mp/objs/net_object.h>

class GrenadeNetObject : public NetObject
{
private:

#ifdef JC_CLIENT
	shared_ptr<class PlayerGrenade> obj;
#endif

	vec3 a2;
	vec3 a3;

	Player* owner = nullptr;

	void destroy_object();

public:

	static constexpr NetObjectType TYPE() { return NetObject_Grenade; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	GrenadeNetObject(NID nid, const TransformTR& transform);

	class PlayerGrenade* get_object() const;

	class ObjectBase* get_object_base() const override;
#else
	GrenadeNetObject(SyncType sync_type, const TransformTR& transform);
#endif
	~GrenadeNetObject();

	void on_spawn() override;
	void on_despawn() override;
	void on_sync() override;
	void on_net_var_change(NetObjectVarType var_type) override;
	void serialize_derived(const Packet* p) override {}
	void deserialize_derived(const Packet* p) override {}
	void serialize_derived_create(const Packet* p) override;
	void deserialize_derived_create(const Packet* p) override;

	void set_owner(Player* v) { owner = v; }
	void set_a2(const vec3& v) { a2 = v; }
	void set_a3(const vec3& v) { a3 = v; }

	Player* get_owner() const { return owner; }

	const vec3& get_a2() const { return a2; }
	const vec3& get_a3() const { return a3; }
};

#define CREATE_GRENADE_NET_OBJECT(...)	JC_ALLOC(GrenadeNetObject, __VA_ARGS__)