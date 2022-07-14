#pragma once

enum ChannelID : unsigned char
{
	ChannelID_PlayerClient,		// for player client related packets (including initialization)
	ChannelID_Chat,				// for chat related packets
	ChannelID_World,			// for world related packets such as day time, weather etc
	ChannelID_Generic,			// for generic/common packets sent every tick or almost every tick
	ChannelID_Max
};