#pragma once

using PacketID = uint8_t;

// player client

DEFINE_ENUM(PlayerClientPacketID, PacketID)
{
	PlayerClientPID_Init,
	PlayerClientPID_Join,
	PlayerClientPID_Nick,
	PlayerClientPID_ObjectInstanceSync,
	PlayerClientPID_ResourceAction,
	PlayerClientPID_ResourcesRefresh,
	PlayerClientPID_RegisterUser,
	PlayerClientPID_LoginUser,
	PlayerClientPID_LogoutUser,
	PlayerClientPID_DebugLog,
	PlayerClientPID_TriggerRemoteEvent,
	PlayerClientPID_Max
};

// check packets

DEFINE_ENUM(CheckPacketID, PacketID)
{
	CheckPID_NetObjects,					// sync the instances for all net objects
};

// chat packets

DEFINE_ENUM(ChatPacketID, PacketID)
{
	ChatPID_Msg,							// chat message
};

// generic packets

DEFINE_ENUM(GenericPacketID, PacketID)
{
	GenericPID_Max
};

DEFINE_ENUM(PlayerPacketID, PacketID)
{
	PlayerPID_StateSync = GenericPID_Max,			// syncs all the critical info of a player
	PlayerPID_Respawn,								// respawns a player
	PlayerPID_DynamicInfo,							// syncs dynamic info such as position, velocity etc
	PlayerPID_StanceAndMovement,					// syncs stance and movement controls
	PlayerPID_SetWeapon,							// sync weapon switching
	PlayerPID_SetVehicle,							// sync player's vehicles assigns
	PlayerPID_EnterExitVehicle,						// sync vehicle entering and exiting
	PlayerPID_VehicleControl,						// sync vehicle controls
	PlayerPID_VehicleHonk,							// sync vehicle honking
	PlayerPID_VehicleEngineState,					// sync vehicle engine state
	PlayerPID_VehicleFire,							// sync vehicle firing
	PlayerPID_VehicleMountedGunFire,				// sync vehicle mounted gun firing
	PlayerPID_Max
};

DEFINE_ENUM(WorldPacketID, PacketID)
{
	WorldPID_SetTime = PlayerPID_Max,			// day cycle set hour time (0.0-24.0)
	WorldPID_SetTimeScale,						// syncs the time scale
	WorldPID_SetPunchForce,						// syncs punch force
	WorldPID_SpawnObject,						// ask and spawn an object in the server and remote players
	WorldPID_SpawnObjectWithPFX,				// ask and spawn an object with a pfx specified in the server and remote players
	WorldPID_DestroyObject,						// used by the client and server to sync destruction of an object
	WorldPID_SetOwnership,						// used to let the players know if their ownership of an object changed (used by the server)
	WorldPID_SyncObject,						// used to sync info about net objects that are not players
	WorldPID_Max
};

// debug packets

DEFINE_ENUM(DbgPacketID, PacketID)
{
	DbgPID_SetTime = WorldPID_Max,
};