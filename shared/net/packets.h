#pragma once

using PacketID = uint32_t;

// player client

enum PlayerClientPacketID : unsigned int
{
	PlayerClientPID_State,					// contains a lot of initialization logic depending on the state
	PlayerClientPID_Connect,
	PlayerClientPID_Disconnect,
	PlayerClientPID_Nick,
	PlayerClientPID_Max
};

// check packets

enum CheckPacketID : unsigned int
{
	CheckPID_NetObjects,					// sync the instances for all net objects (single or broadcast)
	CheckPID_PlayersStaticInfo,				// sync players static info (single or broadcast)
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
	PlayerPID_SyncSpawn = GenericPID_Max,
	PlayerPID_Transform,
	PlayerPID_SetAnim,
	PlayerPID_Max
};

enum DayCyclePacketID : unsigned int
{
	DayCyclePID_SetTime = PlayerPID_Max,	// day cycle set hour time (0.0-24.0)
};