#pragma once

using TcpPacketID = uint16_t;

enum ClientToMsPacketID : TcpPacketID
{
	ClientToMsPacket_ServersInfo,
	ClientToMsPacket_Max,
};

enum ServerToMsPacketID : TcpPacketID
{
	ServerToMsPacket_Verify,
	ServerToMsPacket_Info,
	ServerToMsPacket_Max,
};

enum ClientToServerPacketID : TcpPacketID
{
	ClientToMsPacket_Password = ServerToMsPacket_Max,
	ClientToMsPacket_SyncDefaultFiles,					// to sync the default server files such as the splash etc
	ClientToMsPacket_StartupSync,						// used by the client to let the server know we need to sync every TCP thing such as resources
	ClientToMsPacket_MetadataResourcesList,				// sent by the server to let the client know the resources count and names
	ClientToMsPacket_SyncResource,						// request to let a peer know that it wants to sync a resource
	ClientToMsPacket_ResourceFile,						// packet that contains the resource files that are outdated/new after a sync request
};