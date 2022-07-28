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
	PlayerPID_Respawn = GenericPID_Max,
	PlayerPID_DynamicInfo,
	PlayerPID_StanceAndMovement,
	PlayerPID_SetWeapon,
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