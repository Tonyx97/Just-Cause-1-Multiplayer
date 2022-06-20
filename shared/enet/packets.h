#pragma once

// generic packets

enum GenericPacketID : unsigned int
{
};

enum PlayerPacketID : unsigned int
{
	PlayerPID_SetNickname,
	PlayerPID_SetAnim
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