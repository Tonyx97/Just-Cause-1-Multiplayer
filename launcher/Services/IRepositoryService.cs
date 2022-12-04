using launcher.Models;
using System.Threading.Tasks;

namespace launcher.Services
{
    public interface IRepositoryService
    {
        // Pings the repository and checks if available to perform additional work.
        // True on success (meaning work can be done), false otherwise.
        //
        Task<bool> IsReachableAsync();

        // Fetches internal state of the repository and returns indication
        // if this is initial run (repository was never initialized for
        // current end client) or not.
        //
        Task<bool> IsInitializedAsync();

        // Initializes repository if possible.
        //
        Task<bool> InitializeAsync();

        // Inserts the data from the repository. Returns true on success.
        //
        Task<bool> InsertOrUpdateData(GameSettings gameSettings);

        // Fetches data from the repository. Returns data on success null
        // otherwise.
        //
        Task<GameSettings?> FetchDataAsync();
    }
}
