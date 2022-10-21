#pragma once

#include <shared_mp/objs/net_object.h>

class PickupNetObject : public NetObject
{
private:

	std::string lod;

	uint32_t item_type;

#ifdef JC_CLIENT
	class ItemPickup* obj = nullptr;
#endif

	void destroy_object();

public:

	static constexpr NetObjectType TYPE() { return NetObject_Pickup; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	PickupNetObject(NID nid, const TransformTR& transform);

	class ObjectBase* get_object_base() const override;
#else
	PickupNetObject(SyncType sync_type, const TransformTR& transform);
#endif
	~PickupNetObject();

	void on_spawn() override;
	void on_despawn() override;
	void on_sync() override;
	void on_net_var_change(NetObjectVarType var_type) override;
	void serialize_derived(const Packet* p) override {}
	void deserialize_derived(const Packet* p) override {}
	void serialize_derived_create(const Packet* p) override;
	void deserialize_derived_create(const Packet* p) override;

	void set_item_type(uint32_t v) { item_type = v; }
	void set_lod(const std::string& v) { lod = v; }

	uint32_t get_item_type() const { return item_type; }

	const std::string& get_lod() const { return lod; }
};

#define CREATE_PICKUP_NET_OBJECT(...)	JC_ALLOC(PickupNetObject, __VA_ARGS__)