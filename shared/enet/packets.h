#pragma once

// generic packets

enum GenericPacketID : unsigned int
{
	GenericPID_Max
};

enum PlayerClientPacketID : unsigned int
{
	PlayerClientPID_SetNickname = GenericPID_Max,
	PlayerClientPID_Max
};

enum PlayerPacketID : unsigned int
{
	PlayerPID_SetAnim = PlayerClientPID_Max,
	PlayerPID_Max
};

enum DayCyclePacketID : unsigned int
{
	DayCyclePID_SetTime,
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