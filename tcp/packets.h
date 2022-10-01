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
	ServerToMsPacket_Info,
	ServerToMsPacket_Max,
};

enum ClientToServerPacketID : TcpPacketID
{
	ClientToMsPacket_Password = ServerToMsPacket_Max,
	ClientToMsPacket_SyncDefaultFiles,
	ClientToMsPacket_SyncResource,
	ClientToMsPacket_ResourceFiles,
};