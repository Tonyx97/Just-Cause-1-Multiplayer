#include <defs/standard.h>

#include <mp/net.h>

#include "vehicle_net.h"

#ifdef JC_CLIENT
#include <game/sys/core/factory_system.h>

#include <game/object/base/comps/physical.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/vehicle/comps/vehicle_seat.h>
#include <game/object/weapon/weapon.h>

VehicleNetObject::VehicleNetObject(NID nid, const TransformTR& transform)
{
	set_nid(nid);
	set_transform(transform);
	//set_transform_timer(5000);
	set_velocity_timer(500);
}

ObjectBase* VehicleNetObject::get_object_base() const
{
	return get_object();
}

void VehicleNetObject::fire()
{
	if (!obj)
		return;

	constexpr auto MEMBERS_COUNT = member_counter<FireInfo>::value;

	for (const auto& info : fire_info)
		if (const auto weapon = obj->get_weapon(info.index))
		{
			weapon->set_last_shot_time(jc::nums::MAXF);
			weapon->force_fire();
			weapon->set_enabled(true);
			weapon->update();
		}
}

void VehicleNetObject::fire_mounted_gun()
{
	if (!obj)
		return;

	if (const auto special_seat = obj->get_special_seat())
		if (const auto weapon = special_seat->get_weapon())
		{
			weapon->set_last_shot_time(jc::nums::MAXF);
			weapon->force_fire();
			weapon->set_enabled(true);
			weapon->update();
		}
}

bool VehicleNetObject::warp_to_seat(Player* player, uint8_t seat_type)
{
	if (!obj)
		return false;

	const auto seat = obj->get_seat_by_type(seat_type);

	seat->warp_character(player->get_character(), true);

	return true;
}

bool VehicleNetObject::kick_player(uint8_t seat_type)
{
	if (!obj)
		return false;

	const auto seat = obj->get_seat_by_type(seat_type);

	seat->instant_exit();

	return true;
}

const VehicleNetObject::FireInfo* VehicleNetObject::get_fire_info_from_weapon(Weapon* weapon) const
{
	if (!obj)
		return nullptr;

	for (const auto& info : fire_info)
		if (obj->get_weapon(info.index) == weapon)
			return &info;

	return nullptr;
}

const VehicleNetObject::FireInfoBase* VehicleNetObject::get_mounted_gun_fire_info() const
{
	return &mounted_gun_fire_info;
}
#else
VehicleNetObject::VehicleNetObject(SyncType sync_type, const TransformTR& transform)
{
	set_sync_type(sync_type);
	set_transform(transform);
}
#endif

VehicleNetObject::~VehicleNetObject()
{
	destroy_object();
}

void VehicleNetObject::destroy_object()
{
	IF_CLIENT_AND_VALID_OBJ_DO([&]() { g_factory->destroy_vehicle(std::exchange(obj, nullptr)); });
}

void VehicleNetObject::on_spawn()
{
#ifdef JC_CLIENT
	obj = g_factory->spawn_vehicle(get_object_id(), get_position());

	check(obj, "Could not create vehicle");

	log(PURPLE, "{} {:x} spawned now {:x} at {:.2f} {:.2f} {:.2f}", typeid(*obj).name(), get_nid(), ptr(obj), get_position().x, get_position().y, get_position().z);
#endif
}

void VehicleNetObject::on_despawn()
{
	destroy_object();
}

void VehicleNetObject::on_sync()
{
}

void VehicleNetObject::on_net_var_change(NetObjectVarType var_type)
{
#ifdef JC_CLIENT
	IF_CLIENT_AND_VALID_OBJ_DO([&]()
	{
		switch (var_type)
		{
		case NetObjectVar_Transform:
		case NetObjectVar_Position:
		case NetObjectVar_Rotation:		obj->set_transform(Transform(get_position(), get_rotation())); break;
		case NetObjectVar_Velocity:		obj->get_physical()->set_velocity(get_velocity()); break;
		case NetObjectVar_Health:		obj->set_hp(get_hp()); break;
		case NetObjectVar_MaxHealth:	obj->set_max_hp(get_max_hp()); break;
		}
	});
#else
	switch (var_type)
	{
	case NetObjectVar_Transform:
	case NetObjectVar_Position:
	case NetObjectVar_Rotation:
	{
		// when we receive the transform from the streamer of this vehicle then update position
		// for all players so at least we have a valid position for all players inside the vehicle

		for_each_player([&](uint8_t, Player* player)
		{
			player->set_transform(get_transform());
		});

		break;
	}
	}
#endif
}

void VehicleNetObject::serialize_derived(const Packet* p)
{
#ifdef JC_CLIENT
#else
	p->add(engine_state);
	p->add(color);
	p->add(faction);
#endif
}

void VehicleNetObject::deserialize_derived(const Packet* p)
{
#ifdef JC_CLIENT
	const auto _engine_state = p->get_bool();
	const auto _color = p->get_u32();
	const auto _faction = p->get_i32();

	log(GREEN, "{} {:x} {}", _engine_state, _color, _faction);

	set_engine_state(_engine_state);
	set_color(_color);
	set_faction(_faction);
#else
#endif
}

void VehicleNetObject::set_control_info(float c0, float c1, float c2, float c3, bool braking)
{
#ifdef JC_CLIENT
	if (!sync_this_tick)
		sync_this_tick = (
			c0 != control_info.c0 ||
			c1 != control_info.c1 ||
			c2 != control_info.c2 ||
			c3 != control_info.c3 ||
			braking != control_info.braking);
#endif

	control_info.c0 = c0;
	control_info.c1 = c1;
	control_info.c2 = c2;
	control_info.c3 = c3;
	control_info.braking = braking;
}

void VehicleNetObject::set_control_info(const ControlInfo& v)
{
	set_control_info(v.c0, v.c1, v.c2, v.c3, v.braking);
}

void VehicleNetObject::set_weapon_info(uint32_t index, uint32_t type)
{
	weapon_index = index;
	weapon_type = type;

	IF_CLIENT_AND_VALID_OBJ_DO([&]()
	{
		obj->set_current_weapon_index(index);
		obj->set_current_weapon_type(type);
	});
}

void VehicleNetObject::set_fire_info(const std::vector<FireInfo>& v)
{
	fire_info = v;
}

void VehicleNetObject::set_mounted_gun_fire_info(const FireInfoBase& v)
{
	mounted_gun_fire_info = v;
}

void VehicleNetObject::set_engine_state(bool v SYNC_PARAM)
{
	engine_state = v;

	SYNC_IF_TRUE(PlayerPID_VehicleDynamicInfo, ChannelID_Generic, this, VehicleDynInfo_EngineState, v);
	IF_CLIENT_AND_VALID_OBJ_DO([&]() { obj->set_engine_state(v); });
}

void VehicleNetObject::set_color(uint32_t v SYNC_PARAM)
{
	color = v;

	SYNC_IF_TRUE(PlayerPID_VehicleDynamicInfo, ChannelID_Generic, this, VehicleDynInfo_Color, v);
	IF_CLIENT_AND_VALID_OBJ_DO([&]() { obj->set_color(v); });
}

void VehicleNetObject::set_faction(int32_t v SYNC_PARAM)
{
	faction = v;

	SYNC_IF_TRUE(PlayerPID_VehicleDynamicInfo, ChannelID_Generic, this, VehicleDynInfo_Faction, v);
	IF_CLIENT_AND_VALID_OBJ_DO([&]() { obj->set_faction(v); });
}

void VehicleNetObject::set_player(uint8_t seat_type, Player* player)
{
	players[seat_type] = player;
}

void VehicleNetObject::remove_player(Player* player)
{
	std::erase_if(players, [&](const auto& p)
	{
		return p.second == player;
	});
}

Player* VehicleNetObject::get_player_from_seat(uint8_t seat_type) const
{
	const auto it = players.find(seat_type);
	return it != players.end() ? it->second : nullptr;
}