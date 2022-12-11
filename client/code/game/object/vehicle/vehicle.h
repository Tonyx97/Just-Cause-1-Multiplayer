#pragma once

#include "../action_point_owner/action_point_owner.h"

#include "comps/vehicle_seat.h"

class Vehicle;

namespace jc::vehicle
{
	static constexpr uint32_t DRIVER_SEAT			= 0xA0;
	static constexpr uint32_t PASSENGER_SEAT		= 0xA4;
	static constexpr uint32_t ROOFTOP_SEAT			= 0xA8;
	static constexpr uint32_t SPECIAL_SEAT			= 0xAC;
	static constexpr uint32_t LEFT_DOOR_STATE		= 0xB8;
	static constexpr uint32_t RIGHT_DOOR_STATE		= 0xC8;
	static constexpr uint32_t COLOR_UNK				= 0xD8;
	static constexpr uint32_t COLOR_BODY			= 0xDC;
	static constexpr uint32_t ENGINE_STATE			= 0x231;
	static constexpr uint32_t FACTION				= 0x240;
	static constexpr uint32_t CURRENT_WEAPON_TYPE	= 0x220;
	static constexpr uint32_t CURRENT_WEAPON_INDEX	= 0x224;
	static constexpr uint32_t WEAPONS				= 0x29C;
	static constexpr uint32_t SOUND_COMPONENT		= 0x404;

	namespace fn
	{
		static constexpr uint32_t OPEN_LEFT_DOOR		= 0x62C2C0;
		static constexpr uint32_t OPEN_RIGHT_DOOR		= 0x62D170;
		static constexpr uint32_t DETACH_LEFT_DOOR		= 0x62BE80;
		static constexpr uint32_t IS_LEFT_DOOR_VALID	= 0x62CDF0;
		static constexpr uint32_t IS_LEFT_DOOR_CLOSING	= 0x62CE30;
		static constexpr uint32_t SET_DRIVER			= 0x62B570;
		static constexpr uint32_t SET_FACTION			= 0x854A00;
	}

	namespace vt
	{
		static constexpr uint32_t GET_TYPE				= 37;
		static constexpr uint32_t SET_DRIVER			= 44;
		static constexpr uint32_t GET_VELOCITY			= 48;
		static constexpr uint32_t SET_VELOCITY			= 50;
		static constexpr uint32_t GET_ROOF_SEAT			= 52;
		static constexpr uint32_t GET_DRIVER_SEAT		= 53;
		static constexpr uint32_t GET_PASSENGER_SEAT	= 54;
		static constexpr uint32_t GET_SPECIAL_SEAT		= 55;
		static constexpr uint32_t SET_FACTION			= 56;
		static constexpr uint32_t UNK0					= 57;
		static constexpr uint32_t UNK1					= 58;
		static constexpr uint32_t HONK					= 76;
		static constexpr uint32_t START_ENGINE_SOUND	= 82;
		static constexpr uint32_t STOP_ENGINE_SOUND		= 83;
	}

	namespace hook
	{
		void enable(bool apply);
	}

	bool getting_helicopter_input();

	void dispatch_helicopter_input(int control, float* value);
}

class Weapon;
class Vehicle;

class VehicleWeapon
{
private:
public:

	uint32_t get_type() const { return jc::read<uint32_t>(this); }
	Weapon* get_weapon() const { return jc::read<Weapon*>(this, 0x144); }
	Vehicle* get_vehicle() const { return jc::read<Vehicle*>(this, 0x1F8); }
};

DEFINE_ENUM(VehicleFaction, int)
{
	VehFaction_None,
	VehFaction_Agency = 2,
	VehFaction_Military,
	VehFaction_Police,
	VehFaction_Guerrilla,
	VehFaction_BlackHand,
	VehFaction_Montano,
	VehFaction_Rioja,
	VehFaction_Race = 14
};

DEFINE_ENUM(VehicleTypeID, uint8_t)
{
	VehicleTypeID_None,
	VehicleTypeID_Car,
	VehicleTypeID_MotorBike,
	VehicleTypeID_AirPlane,
	VehicleTypeID_Helicopter,
	VehicleTypeID_Boat,
	VehicleTypeID_Tank,
	VehicleTypeID_MountedGun,
	VehicleTypeID_Submarine,
};

DEFINE_ENUM(VehicleFlags, uint8_t)
{
	VehicleFlag_BlockPassengerToDriver = (1 << 4),
};

DEFINE_ENUM(VehicleDoor, uint8_t)
{
	VehicleDoor_Left,
	VehicleDoor_Right
};

DEFINE_ENUM(VehicleDoorState, uint8_t)
{
	VehicleDoorState_Closed,
	VehicleDoorState_Opening,
	VehicleDoorState_Hanging,
	VehicleDoorState_Closing,
	VehicleDoorState_Destroyed,
};

using vehicle_weapon_fn_t = std::function<void(int, Weapon*, uint32_t)>;

class Vehicle : public ActionPointOwner
{
private:

	void write_engine_state(bool v);
	void start_engine_sound(bool v);

public:

	IMPL_OBJECT_TYPE_ID("CVehicle");

	void set_faction(VehicleFaction v, bool unk = true);
	void set_color(uint32_t v);
	void set_velocity(const vec3& v);
	void honk();
	void set_engine_state(bool v, bool sync = true);
	void set_current_weapon_index(uint32_t v);
	void set_current_weapon_type(uint32_t v);
	void for_each_weapon(const vehicle_weapon_fn_t& fn);
	void for_each_current_weapon(const vehicle_weapon_fn_t& fn);
	void detach_door(uint8_t i);
	void unk0(bool unk);
	void unk1(bool unk);

	bool open_door(uint8_t i, float v = 0.9f);
	bool get_engine_state() const;
	bool is_left_door_valid() const;
	bool is_left_door_closing() const;

	ptr get_sound_component() const;

	uint8_t get_type() const;

	uint32_t get_current_weapon_index() const;
	uint32_t get_current_weapon_type() const;

	u8vec4 get_color() const;

	vec3 get_velocity() const;

	Weapon* get_weapon(int i) const;

	VehicleSeat* get_seat_by_type(uint8_t type) const;
	VehicleSeat* get_roof_seat() const;
	VehicleSeat* get_driver_seat() const;
	VehicleSeat* get_passenger_seat() const;
	VehicleSeat* get_special_seat() const;
};