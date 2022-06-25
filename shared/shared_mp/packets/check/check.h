#pragma once

struct PacketCheck_PlayerStaticInfo : public PacketBase
{
	String<32> nick;

	uint32_t skin = 0u;
};