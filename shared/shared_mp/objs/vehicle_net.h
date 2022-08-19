#pragma once

#include <shared_mp/objs/net_object.h>

class VehicleNetObject : public NetObject
{
private:

#ifdef JC_CLIENT
	class Vehicle* obj = nullptr;
#endif

public:

	static constexpr NetObjectType TYPE() { return NetObject_Vehicle; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	VehicleNetObject(NID nid, const TransformTR& transform);

	class ObjectBase* get_object_base() override;
#else
	VehicleNetObject(SyncType sync_type, const TransformTR& transform);
#endif
	~VehicleNetObject();

	void on_sync() override;
	void on_net_var_change(NetObjectVarType var_type) override;

	bool spawn() override;
};

#define CREATE_VEHICLE_NET_OBJECT(...)	JC_ALLOC(VehicleNetObject, __VA_ARGS__)