using System.Text.Json.Serialization;

namespace launcher.Models
{
    /// <summary>
    /// A class for a query for server information.
    /// </summary>
    /// <param name="PlayerInformation">Gets player information.</param>
    /// 
    public sealed record PlayerQueryResponse([property: JsonPropertyName("results")] PlayerInformation PlayerInformation);

    /// <summary>
    /// A model for player information.
    /// </summary>
    public class PlayerInformation
    {
        public string Name { get; }

        public PlayerInformation(string name)
        {
            Name = name;
        }

    }
}
