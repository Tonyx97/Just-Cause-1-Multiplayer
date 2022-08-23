#pragma once

#include <shared_mp/objs/net_object.h>

enum VehicleEnterExitCommand : uint8_t
{
	VehicleEnterExit_OpenDoor,
	VehicleEnterExit_Enter,
	VehicleEnterExit_Exit,
};

class VehicleNetObject : public NetObject
{
public:

	struct PackedControlInfo
	{
		int8_t c0 = 0,
			   c1 = 0,
			   c2 = 0,
			   c3 = 0;

		bool braking = false;
	};

	// joystick / keyword variables
	//
	struct ControlInfo
	{
		float c0 = 0.f,
			  c1 = 0.f,
			  c2 = 0.f,
			  c3 = 0.f;

		bool braking = false;

		ControlInfo() {}
		ControlInfo(const PackedControlInfo& packed)
		{
			c0 = util::pack::unpack_norm(packed.c0);
			c1 = util::pack::unpack_norm(packed.c1);
			c2 = util::pack::unpack_norm(packed.c2);
			c3 = util::pack::unpack_norm(packed.c3);
			braking = packed.braking;
		}

		PackedControlInfo pack() const
		{
			return
			{
				.c0 = util::pack::pack_norm(c0),
				.c1 = util::pack::pack_norm(c1),
				.c2 = util::pack::pack_norm(c2),
				.c3 = util::pack::pack_norm(c3),
				.braking = braking,
			};
		}
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
	void set_control_info(float c0, float c1, float c2, float c3, bool braking = false);
	void set_control_info(const ControlInfo& v);

	const ControlInfo& get_control_info() const { return control_info; }

	bool spawn() override;
};

#define CREATE_VEHICLE_NET_OBJECT(...)	JC_ALLOC(VehicleNetObject, __VA_ARGS__)