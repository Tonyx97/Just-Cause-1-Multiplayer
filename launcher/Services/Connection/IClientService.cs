using launcher.Helpers.Extensions;
using launcher.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using Wpf.Ui.Controls;

namespace launcher.Services.Connection
{
    public interface IClientService
    {
        Task<byte[]> GetLatestChecksumAsync();
        Task<byte[]> GetLatest();
    }

    public class ClientService : IClientService
    {
        public async Task<byte[]> GetLatestChecksumAsync()
        {
            var master_server_address = "51.77.201.205";
            var master_server_port = 22502;
            var client = new TcpClient();

            try
            {
                await client.ConnectAsync(master_server_address, master_server_port);

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

                var serialized_request_header = packet_header.create_hash_request();

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

                    var packed = (packed_id)read_data[data_offset];
                    // Only supporting server info for now.
                    //
                    if (packed != packed_id.client_to_ms_hash) break;
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

        public async Task<byte[]> GetLatest()
        {
            var master_server_address = "51.77.201.205";
            var master_server_port = 22502;
            var client = new TcpClient();

            try
            {
                await client.ConnectAsync(master_server_address, master_server_port);

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

                var serialized_request_header = packet_header.create_download_request();

                // Send the message to the connected TcpServer.
                //
                client_stream.Write(serialized_request_header, 0, serialized_request_header.Length);

                // We are going to make proper ghetto parsing for chunk TCP downloads here since the file might 
                // be a bigger blob (delta parsing).
                //
                var packet_id_response = new byte[0x2];
                var packet_output = client_stream.Read(packet_id_response, 0, packet_id_response.Length);
                if (packet_output != 0x2) return Array.Empty<byte>();
                var packet_id = (packed_id)BitConverter.ToInt16(packet_id_response);
                if(packet_id != packed_id.client_to_ms_download)
                {
                    // THE FUCK OUTA HERE HEHEHEHEHEHHEHEEH
                    //
                    return Array.Empty<byte>();
                }

                var packet_size_response = new byte[0x4];
                var packet_size_output = client_stream.Read(packet_size_response, 0, packet_size_response.Length);
                if(packet_size_output != 0x4) return Array.Empty<byte>();
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

                if(downloaded_blob.Count != binary_size)
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
}
