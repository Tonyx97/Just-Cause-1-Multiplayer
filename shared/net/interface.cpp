#include <defs/standard.h>

#include "interface.h"

#include <mp/net.h>

#include <shared_mp/objs/all.h>
#include <shared_mp/net_handlers/all.h>

#ifdef JC_CLIENT
#include <game/sys/all.h>
#include <shared_mp/objs/player.h>

#include <game/sys/world/world.h>
#include <game/object/character/character.h>
#endif

#define ENABLE_FAKE_LAG 0

namespace enet
{
	channel_dispatch_t channel_fns[ChannelID_Max] = { nullptr };

	std::vector<Packet> lagged_packets;

	//int fake_lag = 0;

	void call_channel_dispatcher_internal(const Packet& p)
	{
		switch (channel_fns[p.get_channel()](p))
		{
		case PacketRes_NotFound: logt(RED, "Unknown packet received, id = '{}', channel = '{}'", p.get_id(), p.get_channel()); return;
		}
	}
}

#ifdef JC_CLIENT
ENetPeer* enet::GET_CLIENT_PEER()
{
	return g_net->get_peer();
}
#endif

ENetHost* enet::GET_HOST()
{
	return g_net->get_host();
}

void enet::init()
{
	if (enet_initialize() != 0)
		return logt(RED, "Error initializing enet");

	logt(GREEN, "Net initialized");

#ifdef JC_CLIENT
	//
#else
	enet::INIT_NIDS_POOL();
#endif

#if defined(JC_DBG) && ENABLE_FAKE_LAG
	enet::set_fake_lag(0);
#endif
}

void enet::setup_channels()
{
	// player client dispatcher

	enet::add_channel_dispatcher(ChannelID_PlayerClient, [&](const Packet& p)
	{
		switch (auto id = p.get_id())
		{
#ifdef JC_CLIENT
		case PlayerClientPID_ObjectInstanceSync:	return nh::player_client::object_instance_sync(p);
		case PlayerClientPID_DebugLog:				return nh::player_client::debug_log(p);
#else
		case PlayerClientPID_DebugLog:				return nh::player_client::debug_log(p);
		case PlayerClientPID_ResourcesRefresh:		return nh::player_client::resources_refresh(p);
#endif
		case PlayerClientPID_Init:					return nh::player_client::init(p);
		case PlayerClientPID_Nick:					return nh::player_client::nick(p);
		case PlayerClientPID_ResourceAction:		return nh::player_client::resource_action(p);
		case PlayerClientPID_RegisterUser:			return nh::player_client::register_user(p);
		case PlayerClientPID_LoginUser:				return nh::player_client::login_user(p);
		case PlayerClientPID_LogoutUser:			return nh::player_client::logout_user(p);
		}

		return PacketRes_NotFound;
	});

	// chat dispatcher

	enet::add_channel_dispatcher(ChannelID_Chat, [&](const Packet& p)
	{
#ifdef JC_CLIENT
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return PacketRes_NotUsable;
#endif

		switch (auto id = p.get_id())
		{
		case ChatPID_Msg: return nh::chat::msg(p);
		}

		return PacketRes_NotFound;
	});

	// world packet dispatcher

	enet::add_channel_dispatcher(ChannelID_World, [&](const Packet& p)
	{
#ifdef JC_CLIENT
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return PacketRes_NotUsable;
#endif

		switch (auto id = p.get_id())
		{
#ifdef JC_CLIENT
		case WorldPID_SetTime:				return nh::world::day_time(p);
		case WorldPID_SetOwnership:			return nh::world::set_ownership(p);
#else
		case WorldPID_SpawnObject:			return nh::world::spawn_object(p);
#endif
		case WorldPID_SetTimeScale:			return nh::world::time_scale(p);
		case WorldPID_SetPunchForce:		return nh::world::punch_force(p);
		case WorldPID_DestroyObject:		return nh::world::destroy_object(p);
		case WorldPID_SyncObject:			return nh::world::sync_object(p);
		}

		return PacketRes_NotFound;
	});

	// generic packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Generic, [&](const Packet& p)
	{
#ifdef JC_CLIENT
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return PacketRes_NotUsable;
#endif

		switch (auto id = p.get_id())
		{
		case PlayerPID_StateSync:				return nh::player::state_sync(p);
		case PlayerPID_Respawn:					return nh::player::respawn(p);
		case PlayerPID_DynamicInfo:				return nh::player::dynamic_info(p);
		case PlayerPID_StanceAndMovement:		return nh::player::stance_and_movement(p);
		case PlayerPID_SetWeapon:				return nh::player::set_weapon(p);
		case PlayerPID_SetVehicle:				return nh::player::set_vehicle(p);
		case PlayerPID_EnterExitVehicle:		return nh::player::enter_exit_vehicle(p);
		case PlayerPID_VehicleControl:			return nh::vehicle::vehicle_control(p);
		case PlayerPID_VehicleHonk:				return nh::vehicle::vehicle_honk(p);
		case PlayerPID_VehicleEngineState:		return nh::vehicle::vehicle_engine_state(p);
		case PlayerPID_VehicleFire:				return nh::vehicle::vehicle_fire(p);
		case PlayerPID_VehicleMountedGunFire:	return nh::vehicle::vehicle_mounted_gun_fire(p);
		}

		return PacketRes_NotFound;
	});

	// debug packet dispatcher

	enet::add_channel_dispatcher(ChannelID_Debug, [&](const Packet& p)
	{
#ifdef JC_CLIENT
		// if localplayer is not in game then we don't want any of these packets

		if (!g_game_status->is_in_game())
			return PacketRes_NotUsable;
#endif

#ifdef JC_CLIENT
#else
		switch (auto id = p.get_id())
		{
		case DbgPID_SetTime:		return nh::dbg::set_time(p);
		}
#endif

		return PacketRes_NotFound;
	});
}

void enet::add_channel_dispatcher(uint8_t id, const channel_dispatch_t& fn)
{
	channel_fns[id] = fn;
}

void enet::call_channel_dispatcher(const ENetEvent& e)
{
	check(e.channelID >= 0 && e.channelID < ChannelID_Max, "Invalid channel ID");

	Packet p(e);

#if defined(JC_CLIENT) && ENABLE_FAKE_LAG
	if (fake_lag <= 0)
#endif
	{
#ifdef JC_SERVER
		check(g_net->has_player_client(p.get_pc()), "Trying to handle a packet from a player who isn't even added to the list, pid {}, pc {:x}", p.get_id(), ptr(p.get_pc()));
#endif
		call_channel_dispatcher_internal(p);
	}
#if defined(JC_CLIENT) && ENABLE_FAKE_LAG
	else
	{
		// if we are using fake lag then accumulate the packets to execute them later
		// available in client only for now

		lagged_packets.push_back(std::move(p));
	}
#endif
}

#if ENABLE_FAKE_LAG
void enet::set_fake_lag(int value)
{
	fake_lag = value;
}
#endif

void enet::process_lagged_packets()
{
#if defined(JC_CLIENT) && ENABLE_FAKE_LAG
	if (fake_lag <= 0)
		return;

	const auto curr_time = util::time::get_time();

	for (auto it = lagged_packets.begin(); it != lagged_packets.end();)
	{
		auto& p = *it;

		const bool remove = p.get_time() < curr_time - fake_lag;

		if (remove)
		{
			call_channel_dispatcher_internal(p);

			it = lagged_packets.erase(it);
		}
		else ++it;
	}
#endif
}

#if ENABLE_FAKE_LAG
int enet::get_fake_lag()
{
	return fake_lag;
}
#endif