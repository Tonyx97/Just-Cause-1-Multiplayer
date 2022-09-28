#pragma once

using TcpPacketID = uint16_t;

enum SharedMsPacketID : TcpPacketID
{
	SharedMsPacket_Type,
	SharedMsPacket_Max,
};

enum ServerToMsPacketID : TcpPacketID
{
	ServerToMsPacket_Verify = SharedMsPacket_Max,
};

enum ClientToServerPacketID : TcpPacketID
{
	ClientToMsPacket_Todo = SharedMsPacket_Max,
};