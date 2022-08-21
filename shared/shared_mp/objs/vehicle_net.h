#pragma once

#include <shared_mp/objs/net_object.h>

class VehicleNetObject : public NetObject
{
public:

	// joystick / keyword variables
	//
	struct ControlInfo
	{
		float x = 0.f,
			  y = 0.f,
			  forward = 0.f,
			  backward = 0.f;

		bool braking = false;
	};

private:

#ifdef JC_CLIENT
	class Vehicle* obj = nullptr;

	bool sync_this_tick = false;
#endif

	ControlInfo control_info {};

public:

	static constexpr NetObjectType TYPE() { return NetObject_Vehicle; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	VehicleNetObject(NID nid, const TransformTR& transform);

	class Vehicle* get_object() const { return obj; }

	class ObjectBase* get_object_base() override;

	void reset_sync() { sync_this_tick = false; }

	bool should_sync_this_tick() const { return sync_this_tick; }
#else
	VehicleNetObject(SyncType sync_type, const TransformTR& transform);
#endif
	~VehicleNetObject();

	void on_sync() override;
	void on_net_var_change(NetObjectVarType var_type) override;
	void set_control_info(float x, float y, float forward, float backward, bool braking = false);
	void set_control_info(const ControlInfo& v);

	const ControlInfo& get_control_info() const { return control_info; }

	bool spawn() override;
};

#define CREATE_VEHICLE_NET_OBJECT(...)	JC_ALLOC(VehicleNetObject, __VA_ARGS__)