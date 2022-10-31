#pragma once

#include <shared_mp/objs/net_object.h>

class BlipNetObject : public NetObject
{
private:

#ifdef JC_CLIENT
	shared_ptr<class UIMapIcon> obj;
#endif

	void destroy_object();

public:

	static constexpr NetObjectType TYPE() { return NetObject_Blip; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	BlipNetObject(NID nid, const TransformTR& transform);

	class UIMapIcon* get_object() const;

	class ObjectBase* get_object_base() const override;
#else
	BlipNetObject(SyncType sync_type, const TransformTR& transform);
#endif
	~BlipNetObject();

	void on_spawn() override;
	void on_despawn() override;
	void on_sync() override;
	void on_net_var_change(NetObjectVarType var_type) override;
	void serialize_derived(const Packet* p) override {}
	void deserialize_derived(const Packet* p) override {}
	void serialize_derived_create(const Packet* p) override {}
	void deserialize_derived_create(const Packet* p) override {}
};

#define CREATE_BLIP_NET_OBJECT(...)	JC_ALLOC(BlipNetObject, __VA_ARGS__)