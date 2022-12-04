using System;
using System.Runtime.InteropServices;

namespace launcher.Services.Connection
{
    public enum packed_id : UInt16
    {
        client_to_ms_packet_server_info = 0,
        client_to_ms_hash = 1,
        client_to_ms_download = 2
    }

    [StructLayout(LayoutKind.Sequential, Pack = 0)]
    [Serializable]
    public class packet_header
    {
        public packed_id id;
        public UInt32 size;

        // TODO: do proper serialization to binary, fuck that for now.
        //
        public static byte[] create_server_info_request()
        {
            //return new byte[] { 0x2, 0x0, 0x0, 0x0, 0x0, 0x0 };
            return new byte[] { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }; // client_to_ms_packet_server_info
        }
        public static byte[] create_hash_request()
        {
            return new byte[] { 0x1, 0x0, 0x0, 0x0, 0x0, 0x0 }; // client_to_ms_hash
        }
        public static byte[] create_download_request()
        {
            return new byte[] { 0x2, 0x0, 0x0, 0x0, 0x0, 0x0 }; // client_to_ms_download
        }
    }
}
