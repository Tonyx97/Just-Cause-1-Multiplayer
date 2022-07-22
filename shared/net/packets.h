#pragma once

using PacketID = uint32_t;

// player client

enum PlayerClientPacketID : unsigned int
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

enum CheckPacketID : unsigned int
{
	CheckPID_NetObjects,					// sync the instances for all net objects
};

// chat packets

enum ChatPacketID : unsigned int
{
	ChatPID_Msg,							// chat message
};

// generic packets

enum GenericPacketID : unsigned int
{
	GenericPID_Max
};

enum PlayerPacketID : unsigned int
{
	PlayerPID_Spawn = GenericPID_Max,
	PlayerPID_DynamicInfo,
	PlayerPID_StanceAndMovement,
	PlayerPID_Health,
	PlayerPID_Max
};

enum DayCyclePacketID : unsigned int
{
	DayCyclePID_SetTime = PlayerPID_Max,	// day cycle set hour time (0.0-24.0)
};