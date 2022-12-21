using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace launcher.Services.Connection
{
    public interface IClientUpdaterService
    {
        Task<byte[]> GetLatestClientLibraryHash();
        Task<byte[]> GetLatestClientLibrary();
        Task<byte[]> GetLatestInjectionHelperLibraryHash();
        Task<byte[]> GetLatestInjectionHelperLibrary();
    }

    // Remote Updater
    public class ClientUpdaterService : IClientUpdaterService
    {
        // TODO: Extract this to settings, easily change updater address+port
        string updaterAddress = "51.77.201.205";
        int updaterPort = 22502;

        public async Task<byte[]> GetLatestClientLibraryHash()
        {
            var client = new TcpClient();

            try
            {
                await client.ConnectAsync(updaterAddress, updaterPort);

                // We were unable to connect. Is server down?
                //
                if (client.Connected == false)
                {
                    return Array.Empty<byte>();
                }

                // Get a client stream for reading and writing.
                //
                var client_stream = client.GetStream();
                if (client_stream == null)
                {
                    return Array.Empty<byte>();
                }

                var serialized_request_header = packet_header.CreateClientHashRequest();

                // Send the message to the connected TcpServer.
                //
                client_stream.Write(serialized_request_header, 0, serialized_request_header.Length);

                // Allocate some arbitrary memory for now. Make this more dynamic.
                //
                var read_data = new byte[0x1000];

                var output_bytes = client_stream.Read(read_data, 0, read_data.Length);

                var data_offset = (uint)0x0;

                // Ghetto parsing.
                //
                while (true)
                {
                    // We parsed the structure or we got empty response (raw data).
                    //
                    if (data_offset >= output_bytes) break;

                    var packed = (MasterlistPacketID)read_data[data_offset];
                    // Only supporting server info for now.
                    //
                    if (packed != MasterlistPacketID.ClientToMsPacket_ClientHash) break;
                    data_offset += 0x2;

                    // Take 4 bytes to parse first chunk.
                    //
                    var size_of_packet = BitConverter.ToInt32(read_data, (int)data_offset);
                    data_offset += 0x4;

                    // No need to parse if no data.
                    //
                    if (size_of_packet == 0x0) break;

                    // We first get hash length.
                    //
                    var hash_size = (uint)read_data[data_offset];
                    if (hash_size == 0x0) break;
                    data_offset += 0x4;

                    // Parse sha256
                    //
                    byte[] sha256_array = new byte[hash_size];
                    Array.Copy(read_data, data_offset, sha256_array, 0, sha256_array.Length);
                    return sha256_array;

                }
                client_stream.Close();
            }
            catch (Exception)
            {
                // TODO: handle exceptions better and decouple from all to get more precise error back to the UI.
                //
            }
            client.Close();

            return Array.Empty<byte>();
        }

        public async Task<byte[]> GetLatestClientLibrary()
        {
            var client = new TcpClient();

            try
            {
                await client.ConnectAsync(updaterAddress, updaterPort);

                // We were unable to connect. Is server down?
                //
                if (client.Connected == false)
                {
                    return Array.Empty<byte>();
                }

                // Get a client stream for reading and writing.
                //
                var client_stream = client.GetStream();
                if (client_stream == null)
                {
                    return Array.Empty<byte>();
                }

                var serialized_request_header = packet_header.CreateClientDownloadRequest();

                // Send the message to the connected TcpServer.
                //
                client_stream.Write(serialized_request_header, 0, serialized_request_header.Length);

                // We are going to make proper ghetto parsing for chunk TCP downloads here since the file might 
                // be a bigger blob (delta parsing).
                //
                var packet_id_response = new byte[0x2];
                var packet_output = client_stream.Read(packet_id_response, 0, packet_id_response.Length);
                if (packet_output != 0x2) return Array.Empty<byte>();
                var packet_id = (MasterlistPacketID)BitConverter.ToInt16(packet_id_response);
                if (packet_id != MasterlistPacketID.ClientToMsPacket_DownloadClient)
                {
                    // THE FUCK OUTA HERE HEHEHEHEHEHHEHEEH
                    //
                    return Array.Empty<byte>();
                }

                var packet_size_response = new byte[0x4];
                var packet_size_output = client_stream.Read(packet_size_response, 0, packet_size_response.Length);
                if (packet_size_output != 0x4) return Array.Empty<byte>();
                var packet_size = BitConverter.ToInt32(packet_size_response);

                var binary_size_response = new byte[0x4];
                var binary_size_output = client_stream.Read(binary_size_response, 0, binary_size_response.Length);
                if (binary_size_output != 0x4) return Array.Empty<byte>();
                var binary_size = BitConverter.ToInt32(binary_size_response);

                var binary_data = new byte[binary_size];
                var left_to_parse = binary_size;
                var downloaded_blob = new List<byte>();
                while (left_to_parse > 0)
                {
                    var chunk_size = client_stream.Read(binary_data, 0, left_to_parse);
                    left_to_parse -= chunk_size;
                    for (int i = 0; i < chunk_size; i++)
                    {
                        downloaded_blob.Add(binary_data[i]);
                    }
                }
                client_stream.Close();

                if (downloaded_blob.Count != binary_size)
                {
                    // Size sanity check. We expect the same amount of bytes being parsed.
                    //
                    return Array.Empty<byte>();
                }

                //downloaded_blob.RemoveRange(downloaded_blob.Count - 0x201, 0x200);
                return downloaded_blob.ToArray();
            }
            catch (Exception)
            {
                // TODO: handle exceptions better and decouple from all to get more precise error back to the UI.
                //
            }
            client.Close();

            return Array.Empty<byte>();
        }

        public async Task<byte[]> GetLatestInjectionHelperLibraryHash()
        {
            var client = new TcpClient();

            try
            {
                await client.ConnectAsync(updaterAddress, updaterPort);

                // We were unable to connect. Is server down?
                //
                if (client.Connected == false)
                {
                    return Array.Empty<byte>();
                }

                // Get a client stream for reading and writing.
                //
                var client_stream = client.GetStream();
                if (client_stream == null)
                {
                    return Array.Empty<byte>();
                }

                var serialized_request_header = packet_header.CreateInjectorHelperHashRequest();

                // Send the message to the connected TcpServer.
                //
                client_stream.Write(serialized_request_header, 0, serialized_request_header.Length);

                // Allocate some arbitrary memory for now. Make this more dynamic.
                //
                var read_data = new byte[0x1000];

                var output_bytes = client_stream.Read(read_data, 0, read_data.Length);

                var data_offset = (uint)0x0;

                // Ghetto parsing.
                //
                while (true)
                {
                    // We parsed the structure or we got empty response (raw data).
                    //
                    if (data_offset >= output_bytes) break;

                    var packed = (MasterlistPacketID)read_data[data_offset];
                    // Only supporting server info for now.
                    //
                    if (packed != MasterlistPacketID.ClientToMsPacket_InjectorHelperHash) break;
                    data_offset += 0x2;

                    // Take 4 bytes to parse first chunk.
                    //
                    var size_of_packet = BitConverter.ToInt32(read_data, (int)data_offset);
                    data_offset += 0x4;

                    // No need to parse if no data.
                    //
                    if (size_of_packet == 0x0) break;

                    // We first get hash length.
                    //
                    var hash_size = (uint)read_data[data_offset];
                    if (hash_size == 0x0) break;
                    data_offset += 0x4;

                    // Parse sha256
                    //
                    byte[] sha256_array = new byte[hash_size];
                    Array.Copy(read_data, data_offset, sha256_array, 0, sha256_array.Length);
                    return sha256_array;

                }
                client_stream.Close();
            }
            catch (Exception)
            {
                // TODO: handle exceptions better and decouple from all to get more precise error back to the UI.
                //
            }
            client.Close();

            return Array.Empty<byte>();
        }
        public async Task<byte[]> GetLatestInjectionHelperLibrary()
        {
            var client = new TcpClient();

            try
            {
                await client.ConnectAsync(updaterAddress, updaterPort);

                // We were unable to connect. Is server down?
                //
                if (client.Connected == false)
                {
                    return Array.Empty<byte>();
                }

                // Get a client stream for reading and writing.
                //
                var client_stream = client.GetStream();
                if (client_stream == null)
                {
                    return Array.Empty<byte>();
                }

                var serialized_request_header = packet_header.CreateInjectorHelperDownloadRequest();

                // Send the message to the connected TcpServer.
                //
                client_stream.Write(serialized_request_header, 0, serialized_request_header.Length);

                // We are going to make proper ghetto parsing for chunk TCP downloads here since the file might 
                // be a bigger blob (delta parsing).
                //
                var packet_id_response = new byte[0x2];
                var packet_output = client_stream.Read(packet_id_response, 0, packet_id_response.Length);
                if (packet_output != 0x2) return Array.Empty<byte>();
                var packet_id = (MasterlistPacketID)BitConverter.ToInt16(packet_id_response);
                if (packet_id != MasterlistPacketID.ClientToMsPacket_DownloadInjectorHelper)
                {
                    // THE FUCK OUTA HERE HEHEHEHEHEHHEHEEH
                    //
                    return Array.Empty<byte>();
                }

                var packet_size_response = new byte[0x4];
                var packet_size_output = client_stream.Read(packet_size_response, 0, packet_size_response.Length);
                if (packet_size_output != 0x4) return Array.Empty<byte>();
                var packet_size = BitConverter.ToInt32(packet_size_response);

                var binary_size_response = new byte[0x4];
                var binary_size_output = client_stream.Read(binary_size_response, 0, binary_size_response.Length);
                if (binary_size_output != 0x4) return Array.Empty<byte>();
                var binary_size = BitConverter.ToInt32(binary_size_response);

                var binary_data = new byte[binary_size];
                var left_to_parse = binary_size;
                var downloaded_blob = new List<byte>();
                while (left_to_parse > 0)
                {
                    var chunk_size = client_stream.Read(binary_data, 0, left_to_parse);
                    left_to_parse -= chunk_size;
                    for (int i = 0; i < chunk_size; i++)
                    {
                        downloaded_blob.Add(binary_data[i]);
                    }
                }
                client_stream.Close();

                if (downloaded_blob.Count != binary_size)
                {
                    // Size sanity check. We expect the same amount of bytes being parsed.
                    //
                    return Array.Empty<byte>();
                }

                //downloaded_blob.RemoveRange(downloaded_blob.Count - 0x201, 0x200);
                return downloaded_blob.ToArray();
            }
            catch (Exception)
            {
                // TODO: handle exceptions better and decouple from all to get more precise error back to the UI.
                //
            }
            client.Close();

            return Array.Empty<byte>();
        }

    }

    // Local Updater
    public class LocalClientUpdaterService : IClientUpdaterService
    {
        public Task<byte[]> GetLatestClientLibraryHash()
        {
            var checksumBytes = File.ReadAllBytes("local/client.dll.sha256");
            return Task.FromResult(checksumBytes);
        }

        public Task<byte[]> GetLatestClientLibrary()
        {
            var libraryBytes = File.ReadAllBytes("local/client.dll");
            return Task.FromResult(libraryBytes);
        }

        public Task<byte[]> GetLatestInjectionHelperLibraryHash()
        {
            var checksumBytes = File.ReadAllBytes("local/injection_helper.dll.sha256");
            return Task.FromResult(checksumBytes);
        }

        public Task<byte[]> GetLatestInjectionHelperLibrary()
        {
            var libraryBytes = File.ReadAllBytes("local/injection_helper.dll");
            return Task.FromResult(libraryBytes);
        }
    }
}
