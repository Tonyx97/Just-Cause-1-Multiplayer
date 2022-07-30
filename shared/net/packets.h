#pragma once

using PacketID = uint8_t;

// player client

enum PlayerClientPacketID : PacketID
{
	PlayerClientPID_Init,
	PlayerClientPID_Join,
	PlayerClientPID_Quit,
	PlayerClientPID_SyncInstances,
	PlayerClientPID_BasicInfo,
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
	PlayerPID_Max
};

enum DayCyclePacketID : PacketID
{
	DayCyclePID_SetTime = PlayerPID_Max,	// day cycle set hour time (0.0-24.0)
	DayCyclePID_Max
};

// debug packets

enum DbgPacketID : PacketID
{
	DbgPID_SetTime = DayCyclePID_Max,
};