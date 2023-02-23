#include <defs/standard.h>

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

#include "nh_vehicle.h"

// debug

#ifdef JC_CLIENT
#include <game/object/vehicle/vehicle.h>
#endif

PacketResult nh::vehicle::control(const Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return PacketRes_BadArgs;

#ifdef JC_SERVER
	// check if we should update and broadcast the new info

	if (!vehicle_net->is_owned_by(player))
		return PacketRes_NotAllowed;
#endif

	const auto packed_info = p.get<VehicleNetObject::PackedControlInfo>();
	const auto unpacked_info = VehicleNetObject::ControlInfo(packed_info);

	vehicle_net->set_control_info(unpacked_info);

#ifdef JC_SERVER
	p.add_beginning(player);

	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::vehicle::honk(const Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return PacketRes_BadArgs;

#ifdef JC_CLIENT
	const auto vehicle = vehicle_net->get_object();

	vehicle->honk();
#endif

#ifdef JC_SERVER
	if (!vehicle_net->is_owned_by(player))
		return PacketRes_NotAllowed;

	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::vehicle::sirens(const Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return PacketRes_BadArgs;

	const auto enabled = p.get_bool();

#ifdef JC_CLIENT
	const auto vehicle = vehicle_net->get_object();

	vehicle->enable_sirens(enabled);
#endif

#ifdef JC_SERVER
	if (!vehicle_net->is_owned_by(player))
		return PacketRes_NotAllowed;

	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::vehicle::fire(const Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return PacketRes_BadArgs;

#ifdef JC_SERVER
	if (!vehicle_net->is_owned_by(player))
		return PacketRes_NotAllowed;
#endif

	const auto weapon_index = p.get_u8();
	const auto weapon_type = p.get_u8();
	const auto fire_info = p.get<std::vector<VehicleNetObject::FireInfo>>();

	vehicle_net->set_weapon_info(weapon_index, weapon_type);
	vehicle_net->set_fire_info(fire_info);

#ifdef JC_CLIENT
	vehicle_net->fire();
#endif

#ifdef JC_SERVER
	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::vehicle::mounted_gun_fire(const Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return PacketRes_BadArgs;

	const auto fire_info = p.get<VehicleNetObject::FireInfoBase>();

	vehicle_net->set_mounted_gun_fire_info(fire_info);

#ifdef JC_CLIENT
	vehicle_net->fire_mounted_gun();
#endif

#ifdef JC_SERVER
	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::vehicle::dynamic_info(const Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return PacketRes_BadArgs;

	switch (const auto type = p.get_u8())
	{
	case VehicleDynInfo_EngineState:
	{
		vehicle_net->set_engine_state(p.get_bool());
		break;
	}
	case VehicleDynInfo_Color:
	{
		vehicle_net->set_color(p.get_u32());
		break;
	}
	case VehicleDynInfo_Faction:
	{
		vehicle_net->set_faction(p.get_i32());
		break;
	}
	default: log(RED, "Unknown vehicle dynamic info: {}", type);
	}

#ifdef JC_SERVER
	g_net->send_broadcast(pc, p);
#endif

	return PacketRes_Ok;
}