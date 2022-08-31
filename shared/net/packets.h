#pragma once

using PacketID = uint8_t;

// player client

enum PlayerClientPacketID : PacketID
{
	PlayerClientPID_Init,
	PlayerClientPID_Join,
	PlayerClientPID_Quit,
	PlayerClientPID_SyncInstances,
	PlayerClientPID_StartupInfo,
	PlayerClientPID_Nick,
	PlayerClientPID_Max
};

// check packets

enum CheckPacketID : PacketID
{
	CheckPID_NetObjects,					// sync the instances for all net objects
};

// chat packets

enum ChatPacketID : PacketID
{
	ChatPID_Msg,							// chat message
};

// generic packets

enum GenericPacketID : PacketID
{
	GenericPID_Max
};

enum PlayerPacketID : PacketID
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

enum WorldPacketID : PacketID
{
	WorldPID_SetTime = PlayerPID_Max,			// day cycle set hour time (0.0-24.0)
	WorldPID_SetTimeScale,						// syncs the time scale
	WorldPID_SpawnObject,						// ask and spawn an object in the server and remote players
	WorldPID_DestroyObject,						// used by the client and server to sync destruction of an object
	WorldPID_SetOwnership,						// used to let the players know if their ownership of an object changed (used by the server)
	WorldPID_SyncObject,						// used to sync info about net objects that are not players
	WorldPID_Max
};

// debug packets

enum DbgPacketID : PacketID
{
	DbgPID_SetTime = WorldPID_Max,
};