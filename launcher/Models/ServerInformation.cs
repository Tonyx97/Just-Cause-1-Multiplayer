using Microsoft.Toolkit.Mvvm.ComponentModel;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text.Json.Serialization;
using System.Xml.Linq;

namespace launcher.Models
{
    public enum ServerGameMode
    {
        Unknown = 0,
        Freeroam = 1
    }

    public sealed class ServerInformation
    {
        public string Title { get; }
        public string IpAddress { get; }
        public ServerGameMode GameMode { get; }
        public string Discord { get; }
        public IList<PlayerInformation> Players { get; }
        public int PlayerCount { get; }
        public bool IsProtected { get; }
        public uint MaxPlayers { get; }

        public ServerInformation(string title, string ipAddress, ServerGameMode gameMode, string discord, IList<PlayerInformation> players, uint maxPlayers, bool isProtected)
        {
            Title = title;
            IpAddress = ipAddress;
            GameMode = gameMode;
            Discord = discord;
            Players = players;
            PlayerCount = players.Count;
            IsProtected = isProtected;
            MaxPlayers = maxPlayers;
        }
    }
}