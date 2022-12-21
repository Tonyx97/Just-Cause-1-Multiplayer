using System;
using System.Runtime.InteropServices;

namespace launcher.Services.Connection
{
    public enum MasterlistPacketID : UInt16
    {
        ClientToMsPacket_ServersInfo,
        ClientToMsPacket_ClientHash,
        ClientToMsPacket_InjectorHelperHash,
        ClientToMsPacket_DownloadClient,
        ClientToMsPacket_DownloadInjectorHelper
    }

    [StructLayout(LayoutKind.Sequential, Pack = 0)]
    [Serializable]
    public class packet_header
    {
        public MasterlistPacketID id;
        public UInt32 size;

        // TODO: do proper serialization to binary, fuck that for now.
        //
        public static byte[] CreateServersInfoRequest()
        {
            return new byte[] { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }; // ClientToMsPacket_ServersInfo
        }
        public static byte[] CreateClientHashRequest()
        {
            return new byte[] { 0x1, 0x0, 0x0, 0x0, 0x0, 0x0 }; // ClientToMsPacket_ClientHash
        }
        public static byte[] CreateInjectorHelperHashRequest()
        {
            return new byte[] { 0x2, 0x0, 0x0, 0x0, 0x0, 0x0 }; // ClientToMsPacket_InjectorHelperHash
        }
        public static byte[] CreateClientDownloadRequest()
        {
            return new byte[] { 0x3, 0x0, 0x0, 0x0, 0x0, 0x0 }; // ClientToMsPacket_DownloadClient
        }
        public static byte[] CreateInjectorHelperDownloadRequest()
        {
            return new byte[] { 0x4, 0x0, 0x0, 0x0, 0x0, 0x0 }; // ClientToMsPacket_DownloadInjectorHelper
        }
    }
}
