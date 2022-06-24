#pragma once

// generic packets

enum GenericPacketID : unsigned int
{
	GenericPID_Max
};

enum PlayerPacketID : unsigned int
{
	PlayerPID_SetAnim = GenericPID_Max,
	PlayerPID_Max
};

enum DayCyclePacketID : unsigned int
{
	DayCyclePID_SetTime,	// day cycle set hour time (0.0-24.0)
};

// player client

enum PlayerClientPacketID : unsigned int
{
	PlayerClientPID_Connect,
	PlayerClientPID_Disconnect,
	PlayerClientPID_Nick,
	PlayerClientPID_Max
};

// chat packets

enum ChatPacketID : unsigned int
{
	ChatPID_ChatMsg,		// chat message
};

// check packets

enum CheckPacketID : unsigned int
{
	CheckPID_NetObjects,				// sync the instances for all net objects (single or broadcast)
	CheckPID_PlayersStaticInfo,			// sync players static info (single or broadcast)
};

// init packets

enum InitPacketID : unsigned int
{
	InitPID_Init,
};