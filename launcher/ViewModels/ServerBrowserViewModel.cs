using launcher.Models;
using launcher.Services;
using launcher.Services.Connection;
using Microsoft.Toolkit.Mvvm.ComponentModel;
using Microsoft.Toolkit.Mvvm.Input;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Windows.Markup;
using Wpf.Ui.Common.Interfaces;
using Wpf.Ui.Controls;
using Wpf.Ui.Controls.Interfaces;
using Wpf.Ui.Mvvm.Contracts;

namespace launcher.ViewModels
{
    public partial class ServerBrowserViewModel : ObservableObject, INavigationAware
    {
        private bool _isInitialized = false;

        private readonly IServerService _serverService;
        private readonly IRepositoryService _repositoryService;
        private readonly INavigationService _navigationService;
        private readonly IClientService _clientService;

        [ObservableProperty]
        private bool _fetchingServerDataInProgress;

        [ObservableProperty]
        private ObservableCollection<ServerInformation>? _servers;

        [ObservableProperty]
        private ServerInformation? _selectedServer;
        public ServerBrowserViewModel(IServerService serverService, IRepositoryService repositoryService, INavigationService navigationService, IClientService clientService)
        {
            _serverService = serverService;
            _repositoryService = repositoryService;
            _navigationService = navigationService;
            _clientService = clientService;
        }

        public void OnNavigatedTo()
        {
            if (!_isInitialized)
                InitializeViewModel();
        }

        public void OnNavigatedFrom()
        {
        }

        private async void InitializeViewModel()
        {
            FetchingServerDataInProgress = true;
            Servers = new ObservableCollection<ServerInformation>(await _serverService.GetServersAsync());
            FetchingServerDataInProgress = false;

            _isInitialized = true;
        }

        [ICommand]
        private async void JoinServer(ServerInformation? selectedServer)
        {
            if(selectedServer == null) return;

            // We need valid game settings, otherwise we can not properly configure.
            //
            var game_settings = await _repositoryService.FetchDataAsync();
            if(game_settings == null)
            {
                var messageBox = new Wpf.Ui.Controls.MessageBox();
                messageBox.ButtonLeftName = "I Understand";
                messageBox.ButtonLeftClick += MessageBox_ButtonLeftClick;
                messageBox.ButtonRightClick += MessageBox_ButtonLeftClick;

                messageBox.Show("Information", "We can not find information about game settings. Please set up profile first.");
                _navigationService.Navigate(typeof(Views.Pages.GameSettingsPage));
                return;
            }

            // Check for mandatory fields.
            //
            if(game_settings.Username == null || game_settings.Username.Length == 0)
            {
                var messageBox = new Wpf.Ui.Controls.MessageBox();
                messageBox.ButtonLeftName = "I Understand";
                messageBox.ButtonLeftClick += MessageBox_ButtonLeftClick;
                messageBox.ButtonRightClick += MessageBox_ButtonLeftClick;

                messageBox.Show("Information", "We can not find use your username. Please set up profile first.");
                _navigationService.Navigate(typeof(Views.Pages.GameSettingsPage));
                return;
            }

            if (game_settings.GamePath == null || game_settings.GamePath.Length == 0)
            {
                var messageBox = new Wpf.Ui.Controls.MessageBox();
                messageBox.ButtonLeftName = "I Understand";
                messageBox.ButtonLeftClick += MessageBox_ButtonLeftClick;
                messageBox.ButtonRightClick += MessageBox_ButtonLeftClick;

                messageBox.Show("Information", "We can not find your game path. Please set up profile first.");
                _navigationService.Navigate(typeof(Views.Pages.GameSettingsPage));
                return;
            }

            var mod_name = "jcmp_client.dll";

            // Construct mod path.
            //
            var current_dir = Directory.GetCurrentDirectory();
            var client_dir = Path.Combine(current_dir, "client");
            if (Directory.Exists(client_dir) == false)
            {
                Directory.CreateDirectory(client_dir);
            }

            // Check if client needs updating.
            //
            var needs_update = false;
            var mod_path = Path.Combine(client_dir, mod_name);
            if (File.Exists(mod_path) == false)
            {
                needs_update = true;
            }
            else
            {
                using var sha512 = SHA512.Create();
                using var stream = File.OpenRead(mod_path);
                var hash = sha512.ComputeHash(stream);

                var latest_hash = await _clientService.GetLatestChecksumAsync();
                if (latest_hash.SequenceEqual(hash) == false)
                {
                    needs_update = true;
                }
            }

            // Update the mod.
            //
            if (needs_update)
            {
                var binary = await _clientService.GetLatest();

                await File.WriteAllBytesAsync(mod_path, binary);
            }

            // Cross check if file is on the disk, readable, and up to date.
            //
            if (File.Exists(mod_path) == false)
            {
                var messageBox = new Wpf.Ui.Controls.MessageBox();
                messageBox.ButtonLeftName = "I Understand";
                messageBox.ButtonLeftClick += MessageBox_ButtonLeftClick;
                messageBox.ButtonRightClick += MessageBox_ButtonLeftClick;

                messageBox.Show("Error", "We were unable to set up mod. Please contact support person.");
                return;
            }
        }

        private void MessageBox_ButtonLeftClick(object sender, System.Windows.RoutedEventArgs e)
        {
            (sender as Wpf.Ui.Controls.MessageBox)?.Close();
        }
    }
}
