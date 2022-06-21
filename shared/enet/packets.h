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
	DayCyclePID_SetTime,
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
	ChatPID_ChatMsg,
};

// init packets

enum InitPacketID : unsigned int
{
	InitPID_Init,
};