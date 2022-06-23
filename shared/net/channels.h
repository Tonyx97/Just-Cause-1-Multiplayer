#pragma once

enum ChannelID : unsigned char
{
	ChannelID_Generic,			// for generic/common packets sent every tick or almost every tick
	ChannelID_PlayerClient,		// for player client related packets
	ChannelID_Chat,				// for chat related packets
	ChannelID_Check,			// for refreshing/checking/updating net stuff such as player instances etc
	ChannelID_Init,				// for the first packets during the connection
	ChannelID_Max
};