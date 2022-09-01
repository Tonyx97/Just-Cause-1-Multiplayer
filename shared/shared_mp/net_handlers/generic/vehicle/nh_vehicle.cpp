#include <defs/standard.h>

#include <mp/net.h>

#include "nh_vehicle.h"

// debug

#ifdef JC_CLIENT
#include <game/object/vehicle/vehicle.h>
#endif

enet::PacketResult nh::vehicle::vehicle_control(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto player = p.get_net_object<Player>();

	if (!player)
		return enet::PacketRes_BadArgs;
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

#ifdef JC_SERVER
	// check if we should update and broadcast the new info

	if (!vehicle_net->is_owned_by(player))
		return enet::PacketRes_NotAllowed;
#endif

	const auto packed_info = p.get_raw<VehicleNetObject::PackedControlInfo>();
	const auto unpacked_info = VehicleNetObject::ControlInfo(packed_info);

	vehicle_net->set_control_info(unpacked_info);

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleControl, player, vehicle_net, packed_info);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::vehicle::vehicle_honk(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

#ifdef JC_CLIENT
	const auto vehicle = vehicle_net->get_object();

	vehicle->honk();
#endif

#ifdef JC_SERVER
	if (!vehicle_net->is_owned_by(player))
		return enet::PacketRes_NotAllowed;

	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleHonk, vehicle_net);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::vehicle::vehicle_engine_state(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

#ifdef JC_SERVER
	if (!vehicle_net->is_owned_by(player))
		return enet::PacketRes_NotAllowed;
#endif

	const bool state = p.get_bool();

#ifdef JC_CLIENT
	const auto vehicle = vehicle_net->get_object();

	vehicle->set_engine_state(state);
#endif

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleEngineState, vehicle_net, state);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::vehicle::vehicle_fire(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

#ifdef JC_SERVER
	if (!vehicle_net->is_owned_by(player))
		return enet::PacketRes_NotAllowed;
#endif

	const auto weapon_index = p.get_u8();
	const auto weapon_type = p.get_u8();
	const auto fire_info = p.get_vector<VehicleNetObject::FireInfo>();

	vehicle_net->set_weapon_info(weapon_index, weapon_type);
	vehicle_net->set_fire_info(fire_info);

#ifdef JC_CLIENT
	vehicle_net->fire();
#endif

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleFire, vehicle_net, weapon_index, weapon_type, fire_info);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::vehicle::vehicle_mounted_gun_fire(const enet::Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto player = pc->get_player();
#endif

	const auto vehicle_net = p.get_net_object<VehicleNetObject>();

	if (!vehicle_net)
		return enet::PacketRes_BadArgs;

	const auto fire_info = p.get_raw<VehicleNetObject::FireInfoBase>();

	vehicle_net->set_mounted_gun_fire_info(fire_info);

#ifdef JC_CLIENT
	vehicle_net->fire_mounted_gun();
#endif

#ifdef JC_SERVER
	g_net->send_broadcast_reliable(pc, PlayerPID_VehicleMountedGunFire, vehicle_net, fire_info);
#endif

	return enet::PacketRes_Ok;
}