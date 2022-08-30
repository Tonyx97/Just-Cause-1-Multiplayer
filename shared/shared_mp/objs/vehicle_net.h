#pragma once

#include <shared_mp/objs/net_object.h>

#include <game/shared/vehicle_seat.h>

enum VehicleEnterExitCommand : uint8_t
{
	VehicleEnterExit_RequestEnter,
	VehicleEnterExit_SetVehicle,
	VehicleEnterExit_Exit,
	VehicleEnterExit_PassengerToDriver,
	VehicleEnterExit_DriverToRoof,
};

class Weapon;

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

	struct FireInfoBase
	{
		vec3 muzzle,
			 direction;
	};

	struct FireInfo : public FireInfoBase
	{
		int index;

		FireInfo(int index, const vec3& muzzle, const vec3& direction) : index(index)
		{
			this->muzzle = muzzle;
			this->direction = direction;
		}
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

	std::vector<FireInfo> fire_info;

	FireInfoBase mounted_gun_fire_info {};

	uint32_t weapon_index = 0u,
			 weapon_type = 0u;

public:

	static constexpr NetObjectType TYPE() { return NetObject_Vehicle; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	VehicleNetObject(NID nid, const TransformTR& transform);

	class Vehicle* get_object() const { return obj; }

	class ObjectBase* get_object_base() override;

	void fire();
	void fire_mounted_gun();
	void reset_sync() { sync_this_tick = false; }

	bool should_sync_this_tick() const { return sync_this_tick; }

	const FireInfo* get_fire_info_from_weapon(Weapon* weapon) const;
	const FireInfoBase* get_mounted_gun_fire_info() const;
#else
	VehicleNetObject(SyncType sync_type, const TransformTR& transform);
#endif
	~VehicleNetObject();

	void on_sync() override;
	void on_net_var_change(NetObjectVarType var_type) override;
	void set_control_info(float c0, float c1, float c2, float c3, bool braking = false);
	void set_control_info(const ControlInfo& v);
	void set_weapon_info(uint32_t index, uint32_t type);
	void set_fire_info(const std::vector<FireInfo>& v);
	void set_mounted_gun_fire_info(const FireInfoBase& v);

	uint32_t get_weapon_index() const { return weapon_index; }
	uint32_t get_weapon_type() const { return weapon_type; }

	const ControlInfo& get_control_info() const { return control_info; }

	bool spawn() override;
};

#define CREATE_VEHICLE_NET_OBJECT(...)	JC_ALLOC(VehicleNetObject, __VA_ARGS__)