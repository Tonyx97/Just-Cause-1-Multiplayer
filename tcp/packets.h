#pragma once

using TcpPacketID = uint16_t;

enum ServerToMsPacketID : TcpPacketID
{
	ServerToMsPacket_Verify,
};

enum ClientToServerPacketID : TcpPacketID
{

};