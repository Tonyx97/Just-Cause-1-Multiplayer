#pragma once

#include <shared_mp/objs/net_object.h>

#include <game/shared/vehicle_seat.h>

DEFINE_ENUM(VehicleEnterExitCommand, uint8_t)
{
	VehicleEnterExit_RequestEnter,
	VehicleEnterExit_SetVehicle,
	VehicleEnterExit_Exit,
	VehicleEnterExit_PassengerToDriver,
	VehicleEnterExit_DriverToRoof,
	VehicleEnterExit_RoofToDriver,
};

DEFINE_ENUM(VehicleDynamicInfoID, uint8_t)
{
	VehicleDynInfo_EngineState,
	VehicleDynInfo_Color,
	VehicleDynInfo_Faction,
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

	struct FireInfo
	{
		FireInfoBase base;

		int index;
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

	void destroy_object();

private:

	std::string ee_name;

#ifdef JC_CLIENT
	shared_ptr<class Vehicle> obj;

	bool sync_this_tick = false;
#endif

	std::unordered_map<uint8_t, Player*> players;

	ControlInfo control_info {};

	std::vector<FireInfo> fire_info;

	FireInfoBase mounted_gun_fire_info {};

	uint32_t color = 0xFFFFFFFF;

	int32_t faction = 0;

	uint32_t weapon_index = 0u,
			 weapon_type = 0u;

	bool engine_state = false;

public:

	static constexpr NetObjectType TYPE() { return NetObject_Vehicle; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	VehicleNetObject(NID nid, const TransformTR& transform);

	class Vehicle* get_object() const;

	class ObjectBase* get_object_base() const override;

	void fire();
	void fire_mounted_gun();
	void reset_sync() { sync_this_tick = false; }

	bool warp_to_seat(Player* player, uint8_t seat_type);
	bool kick_player(uint8_t seat_type);
	bool should_sync_this_tick() const { return sync_this_tick; }

	const FireInfo* get_fire_info_from_weapon(Weapon* weapon) const;
	const FireInfoBase* get_mounted_gun_fire_info() const;
#else
	VehicleNetObject(SyncType sync_type, const TransformTR& transform);
#endif
	~VehicleNetObject();

	void on_spawn() override;
	void on_despawn() override;
	void on_sync() override;
	void on_net_var_change(NetObjectVarType var_type) override;
	void serialize_derived(const Packet* p) override;
	void deserialize_derived(const Packet* p) override;
	void serialize_derived_create(const Packet* p) override;
	void deserialize_derived_create(const Packet* p) override;

	void set_ee(const std::string& v) { ee_name = v; }
	void set_control_info(float c0, float c1, float c2, float c3, bool braking = false);
	void set_control_info(const ControlInfo& v);
	void set_weapon_info(uint32_t index, uint32_t type);
	void set_fire_info(const std::vector<FireInfo>& v);
	void set_mounted_gun_fire_info(const FireInfoBase& v);
	void set_engine_state(bool v SYNC_PARAM_DECL);
	void set_color(uint32_t v SYNC_PARAM_DECL);
	void set_faction(int32_t v SYNC_PARAM_DECL);

	template <typename Fn>
	void for_each_player(const Fn& fn)
	{
		for (const auto& [seat_type, player] : players)
			fn(seat_type, player);
	}

	/**
	* assign a player to a seat, this overrides the current player
	* in the seat
	* @param seat_type The seat type
	* @param player The player to be assigned to
	*/
	void set_player(uint8_t seat_type, Player* player);

	/**
	* deassign a player from a seat, if another player was assigned to
	* the seat then this fn will do nothing (which is what it should do)
	* @param player The player to be deassigned from
	*/
	void remove_player(Player* player);

	bool get_engine_state() const { return engine_state; }

	int32_t get_faction() const { return faction; }

	uint32_t get_weapon_index() const { return weapon_index; }
	uint32_t get_weapon_type() const { return weapon_type; }
	uint32_t get_color() const { return color; }

	size_t get_players_count() const { return players.size(); }

	Player* get_player_from_seat(uint8_t seat_type) const;

	const ControlInfo& get_control_info() const { return control_info; }

	const std::string& get_ee() const { return ee_name; }

	bool has_players() const { return !players.empty(); }
};

#define CREATE_VEHICLE_NET_OBJECT(...)	JC_ALLOC(VehicleNetObject, __VA_ARGS__)