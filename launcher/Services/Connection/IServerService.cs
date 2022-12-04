using launcher.Models;
using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace launcher.Services.Connection
{
    public interface IServerService
    {
        Task<IList<ServerInformation>> GetServersAsync();
    }
    public class ServerService : IServerService
    {
        public async Task<IList<ServerInformation>> GetServersAsync()
        {
            // TODO: Improve raw blob parsing and make parsing more dynamic. This is POC.
            //
            var servers = new List<ServerInformation>();

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
                    return servers;
                }

                // Get a client stream for reading and writing.
                //
                var client_stream = client.GetStream();
                if (client_stream == null)
                {
                    return servers;
                }

                var serialized_request_header = packet_header.create_server_info_request();

                // Send the message to the connected TcpServer.
                //
                client_stream.Write(serialized_request_header, 0, serialized_request_header.Length);

                // Allocate some arbitrary memory for now. Make this more dynamic.
                //
                var read_data = new byte[0x1000];

                var output_bytes = client_stream.Read(read_data, 0, read_data.Length);

                var data_offset = 0x0;

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
                    if (packed != packed_id.client_to_ms_packet_server_info) break;
                    data_offset += 0x2;

                    // Take 4 bytes to parse first chunk.
                    //
                    var size_of_packet = BitConverter.ToInt32(read_data, (int)data_offset);
                    data_offset += 0x4;

                    // No need to parse if no data.
                    //
                    if (size_of_packet == 0x0) break;

                    // We first get server length.
                    //
                    var server_count = BitConverter.ToInt32(read_data, (int)data_offset);
                    data_offset += 0x4;

                    // No need to parse server data if size is zero.
                    //
                    if (server_count == 0x0) break;

                    // Parse dynamic data for servers.
                    //
                    for (var i = 0; i < server_count; i++)
                    {
                        var server_ip_length = BitConverter.ToInt32(read_data, (int)data_offset);
                        data_offset += 0x4;

                        string server_ip = "127.0.0.1"; // Used as a fall back.
                        if (server_ip_length > 0)
                        {
                            server_ip = System.Text.Encoding.UTF8.GetString(read_data, (int)data_offset, (int)server_ip_length);
                        }
                        data_offset += server_ip_length;

                        var server_name_length = BitConverter.ToInt32(read_data, (int)data_offset);
                        data_offset += 0x4;
                        string server_name = "Unknown server name"; // Used as a fall back.
                        if (server_name_length > 0)
                        {
                            server_name = System.Text.Encoding.UTF8.GetString(read_data, (int)data_offset, (int)server_name_length);
                        }
                        data_offset += server_name_length;

                        var server_discord_length = BitConverter.ToInt32(read_data, (int)data_offset);
                        data_offset += 0x4;
                        string server_discord = ""; // Used as a fall back.
                        if (server_discord_length > 0)
                        {
                            server_discord = System.Text.Encoding.UTF8.GetString(read_data, (int)data_offset, (int)server_discord_length);
                        }
                        data_offset += server_discord_length;

                        var server_community_length = BitConverter.ToInt32(read_data, (int)data_offset);
                        data_offset += 0x4;
                        string server_community = ""; // Used as a fall back.
                        if (server_community_length > 0)
                        {
                            server_community = System.Text.Encoding.UTF8.GetString(read_data, (int)data_offset, (int)server_community_length);
                        }
                        data_offset += server_community_length;

                        var server_gamemode_length = BitConverter.ToInt32(read_data, (int)data_offset);
                        data_offset += 0x4;
                        string server_gamemode = ""; // Used as a fall back.
                        if (server_gamemode_length > 0)
                        {
                            server_gamemode = System.Text.Encoding.UTF8.GetString(read_data, (int)data_offset, (int)server_gamemode_length);
                        }
                        data_offset += server_gamemode_length;

                        var server_players = new List<PlayerInformation>();
                        // We first get server length.
                        //
                        var server_player_count = BitConverter.ToInt32(read_data, (int)data_offset);
                        data_offset += 0x4;
                        // Parse players.
                        for (var j = 0; j < server_player_count; j++)
                        {

                            var player_namee_length = BitConverter.ToInt32(read_data, (int)data_offset);
                            data_offset += 0x4;
                            string player_name = ""; // Used as a fall back.
                            if (player_namee_length > 0)
                            {
                                player_name = System.Text.Encoding.UTF8.GetString(read_data, (int)data_offset, (int)player_namee_length);
                                PlayerInformation player_info = new PlayerInformation(player_name);
                                server_players.Add(player_info);
                            }
                            data_offset += player_namee_length;
                        }

                        var server_refreshrate = (uint)read_data[data_offset];
                        data_offset += 0x4;

                        var server_password_protected = BitConverter.ToBoolean(read_data, (int)data_offset);
                        data_offset += 0x1;

                        var parsed_server = new ServerInformation(server_name, server_ip, ServerGameMode.Freeroam, server_discord, server_players, server_password_protected);
                        servers.Add(parsed_server);
                    }
                }
                client_stream.Close();
            }
            catch (Exception)
            {
                // TODO: handle exceptions better and decouple from all to get more precise error back to the UI.
                //
            }
            client.Close();

            return servers;
        }
    }

    // Mock service for testing purposes.
    //
    public class ServerServiceMock : IServerService
    {
        public async Task<IList<ServerInformation>> GetServersAsync()
        {
            // fake web request.
            //
            await Task.Delay(5000);
            var rnd = new Random();
            var playerCount = rnd.Next(20);
            var dummyPlayers = new List<PlayerInformation>();
            var dummyPlayers2 = new List<PlayerInformation>();
            for (int j = 0; j < playerCount; j++)
            {
                var playerName = "Player name " + j;
                dummyPlayers.Add(new PlayerInformation(playerName));
            }
            playerCount = rnd.Next(20);
            for (int j = 0; j < playerCount; j++)
            {
                var playerName = "Player name " + j;
                dummyPlayers2.Add(new PlayerInformation(playerName));
            }

            var servers = new List<ServerInformation>();

            var dummyServer = new ServerInformation("Official MP server", "156.256.71.3", ServerGameMode.Freeroam, "https://discord.gg/dq33yEyjv1", dummyPlayers, true);
            var dummyServer2 = new ServerInformation("Official MP server 2", "156.256.71.4", ServerGameMode.Freeroam, "", dummyPlayers2, false);

            servers.Add(dummyServer);
            servers.Add(dummyServer2);

            return servers;
        }
    }
}
