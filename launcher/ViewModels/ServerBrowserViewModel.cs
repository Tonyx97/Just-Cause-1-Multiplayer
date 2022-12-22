using launcher.Models;
using launcher.Services;
using launcher.Services.Connection;
using launcher.Views.Pages;
using Microsoft.Extensions.Logging;
using Microsoft.Toolkit.Mvvm.ComponentModel;
using Microsoft.Toolkit.Mvvm.Input;
using Microsoft.Win32;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.Cryptography;
using System.Threading.Tasks;
using Wpf.Ui.Common.Interfaces;
using Wpf.Ui.Controls.Interfaces;
using Wpf.Ui.Mvvm.Contracts;
using MessageBox = Wpf.Ui.Controls.MessageBox;

namespace launcher.ViewModels
{
    public partial class ServerBrowserViewModel : ObservableObject, INavigationAware
    {
        public bool IsKillGameButtonEnabled { get; set; }

        private readonly ILogger<ServerBrowserViewModel> _logger;
        private readonly IServerListService _serverService;
        private readonly IClientExecutionService _clientExecutionService;
        private readonly IRepositoryService _repositoryService;
        private readonly INavigationService _navigationService;
        private readonly IClientUpdaterService _clientUpdaterService;
        private readonly IDialogControl _dialogControl;

        [ObservableProperty]
        private bool _fetchingServerDataInProgress;

        [ObservableProperty]
        private ObservableCollection<ServerInformation>? _servers;

        [ObservableProperty]
        private ServerInformation? _selectedServer;
        public ServerBrowserViewModel(ILogger<ServerBrowserViewModel> logger, IServerListService serverService, IRepositoryService repositoryService, INavigationService navigationService, IClientUpdaterService clientUpdaterService, IClientExecutionService clientExecutionService, IDialogService dialogService)
        {
            _logger = logger;
            _serverService = serverService;
            _repositoryService = repositoryService;
            _navigationService = navigationService;
            _clientUpdaterService = clientUpdaterService;
            _clientExecutionService = clientExecutionService;
            _dialogControl = dialogService.GetDialogControl();
        }

        public void OnNavigatedTo()
        {
            RefreshServers();
        }

        public void OnNavigatedFrom()
        {
        }

        [ICommand]
        private async void RefreshServers()
        {
            FetchingServerDataInProgress = true;
            Servers = new ObservableCollection<ServerInformation>(await _serverService.GetServerList());
            FetchingServerDataInProgress = false;
        }

        [ICommand]
        private void KillGame()
        {
            if (_clientExecutionService.Process != null)
            {
                _clientExecutionService.Process.Kill();
            }
        }

        [ICommand]
        [MethodImpl(MethodImplOptions.NoOptimization)]
        private async void JoinServer(ServerInformation? selectedServer)
        {
            // Can't join a server if we didn't select one
            if (selectedServer is null) return;

			var jcmpKey = "HKEY_CURRENT_USER\\Software\\JCMP\\Game";
			Registry.SetValue(jcmpKey, "ip", selectedServer.IpAddress);

			// Handle if game settings, username or gamepath were not set up.
			GameSettings ? gameSettings = await _repositoryService.FetchDataAsync();

            if (gameSettings is null)
            {
                ShowInvalidGameSettingsPopup();
                return;
            }

            if (string.IsNullOrEmpty(gameSettings.Username))
            {
                ShowInvalidUsernamePopup();
                return;
            }

            if (string.IsNullOrEmpty(gameSettings.GamePath))
            {
                ShowInvalidGamePathPopup();
                return;
            }

            string? gameDirectoryPath = Path.GetDirectoryName(gameSettings.GamePath);
            if (string.IsNullOrEmpty(gameDirectoryPath) || !Directory.Exists(gameDirectoryPath))
            {
                ShowInvalidGamePathPopup();
                return;
            }

            // Find Steam install dir (for Steam.dll)
            string steamDirPath = GetSteamInstallationDirectoryPath();

            if (string.IsNullOrEmpty(steamDirPath))
            {
                ShowCantFindSteamInstallDirPopup();
                return;
            }

            string steamLibraryFilePath = Path.Combine(steamDirPath, "Steam.dll");

            if (!File.Exists(steamLibraryFilePath))
            {
                ShowCantFindSteamLibraryInstallDirPopup();
                return;
            }

            // Client Updater Module

            bool moduleUpdateSuccesful = await UpdateClientModules(gameDirectoryPath, steamLibraryFilePath);
            if (!moduleUpdateSuccesful)
            {
                return;
            }

            // Launch the game

            ProcessStartInfo processStartInfo = new ProcessStartInfo();
            processStartInfo.FileName = gameSettings.GamePath;
            processStartInfo.WorkingDirectory = gameDirectoryPath;

            IsKillGameButtonEnabled = true;

            Process? process = Process.Start(processStartInfo);

            process.Exited += OnExitEventHandler;
            
            if (process != null)
                _clientExecutionService.LockLauncher(process);

            ShowGameLauncherPopup();

            _navigationService.Navigate(typeof(ServerBrowserPage));
        }

        public void OnExitEventHandler(object? sender, EventArgs e)
        {
            IsKillGameButtonEnabled = false;
            _clientExecutionService.ForceUnlockLauncher();
        }

        private void ShowGameLauncherPopup()
        {
            // TODO
        }

        private async Task<bool> UpdateClientModules(string gameDirectoryPath, string steamLibraryFilePath)
        {
            string injectorLibraryFileName = "dinput8.dll";
            string clientLibraryFileName = "jcmp_client.dll";
            string launcherDirectoryPath = Directory.GetCurrentDirectory();

            // Create "download" directory if doesn't exists
            string downloadDirectoryPath = Path.Combine(launcherDirectoryPath, "download");

            if (!Directory.Exists(downloadDirectoryPath))
            {
                try
                {
                    Directory.CreateDirectory(downloadDirectoryPath);
                }
                catch (Exception exception)
                {
                    _logger.LogError(exception.Message);
                    ShowCantCreateDownloadDirectoryPopup();
                    return false;
                }
            }

            // Injector library file path (./download/dinput8.dll)
            string injectorLibraryFilePath = Path.Combine(downloadDirectoryPath, injectorLibraryFileName);

            // Injector library latest hash (retrieved from client updater)
            byte[] injectorLibraryLatestHash = await _clientUpdaterService.GetLatestInjectionHelperLibraryHash();

            // Download injector library updates if necessary
            bool injectorIsUpToDate = await EnsureLibraryIsUpToDate(injectorLibraryFilePath, injectorLibraryLatestHash, _clientUpdaterService.GetLatestInjectionHelperLibrary);
            if (!injectorIsUpToDate)
            {

                return false;
            }

            // Client Library (jcmp_client.dll)

            // Client library file path (./download/jcmp_client.dll)
            string clientLibraryFilePath = Path.Combine(downloadDirectoryPath, clientLibraryFileName);

            // Client library latest hash (retrieved from client updater)
            byte[] clientLibraryLatestHash = await _clientUpdaterService.GetLatestClientLibraryHash();

            // Download client library updates if necessary
            await EnsureLibraryIsUpToDate(clientLibraryFilePath, clientLibraryLatestHash, _clientUpdaterService.GetLatestClientLibrary);

            // Update library files in game directory

            string gameDirectoryInjectorLibraryFilePath = Path.Combine(gameDirectoryPath, injectorLibraryFileName);
            _logger.LogDebug($"Target injector library file path: {gameDirectoryInjectorLibraryFilePath}");

            string gameDirectoryClientLibraryFilePath = Path.Combine(gameDirectoryPath, clientLibraryFileName);
            _logger.LogDebug($"Target client library file path: {gameDirectoryClientLibraryFilePath}");

            string gameDirectorySteamLibraryFilePath = Path.Combine(gameDirectoryPath, "Steam.dll");
            _logger.LogDebug($"Target steam library file path: {gameDirectorySteamLibraryFilePath}");

            try
            {
                File.Copy(injectorLibraryFilePath, gameDirectoryInjectorLibraryFilePath, true);
                File.Copy(clientLibraryFilePath, gameDirectoryClientLibraryFilePath, true);
                File.Copy(steamLibraryFilePath, gameDirectorySteamLibraryFilePath, true);
            }
            catch (Exception exception)
            {
                _logger.LogError(exception.Message);
                ShowCantInstallNeededLibraries();
                return false;
            }

            return true;
        }

        private void SetupUriScheme()
        {
            RegistryKey regkey = Registry.ClassesRoot.OpenSubKey("jcmp");
        }

        private void ShowCantFindSteamLibraryInstallDirPopup()
        {
            MessageBox messageBox = new MessageBox();

            messageBox.ButtonLeftName = "I Understand";
            messageBox.ButtonLeftClick += CloseMessageBoxEventHandler;
            messageBox.ButtonRightClick += CloseMessageBoxEventHandler;

            messageBox.Show("Error", "We could not find Steam library. Please contact support.");

            _navigationService.Navigate(typeof(ServerBrowserPage));
        }

        private void ShowCantFindSteamInstallDirPopup()
        {
            MessageBox messageBox = new MessageBox();

            messageBox.ButtonLeftName = "I Understand";
            messageBox.ButtonLeftClick += CloseMessageBoxEventHandler;
            messageBox.ButtonRightClick += CloseMessageBoxEventHandler;

            messageBox.Show("Error", "We could not find your Steam installation. Install Steam. If this problem persists, contact support.");

            _navigationService.Navigate(typeof(ServerBrowserPage));
        }

        private void ShowInvalidGameSettingsPopup()
        {
            MessageBox messageBox = new MessageBox();

            messageBox.ButtonLeftName = "I Understand";
            messageBox.ButtonLeftClick += CloseMessageBoxEventHandler;
            messageBox.ButtonRightClick += CloseMessageBoxEventHandler;

            messageBox.Show("Information", "We could not find your game settings. Please set up your username and game path first.");

            _navigationService.Navigate(typeof(GameSettingsPage));
        }

        private void ShowInvalidGamePathPopup()
        {
            MessageBox messageBox = new MessageBox();

            messageBox.ButtonLeftName = "I Understand";
            messageBox.ButtonLeftClick += CloseMessageBoxEventHandler;
            messageBox.ButtonRightClick += CloseMessageBoxEventHandler;

            messageBox.Show("Warning", "We could not find your game path. Please set up your game path first.");

            _navigationService.Navigate(typeof(GameSettingsPage));
        }

        private void ShowInvalidUsernamePopup()
        {
            MessageBox messageBox = new MessageBox();

            messageBox.ButtonLeftName = "I Understand";
            messageBox.ButtonLeftClick += CloseMessageBoxEventHandler;
            messageBox.ButtonRightClick += CloseMessageBoxEventHandler;

            messageBox.Show("Information", "We could not find your username. Please set up your username first.");

            _navigationService.Navigate(typeof(GameSettingsPage));
        }

        private void ShowCantInstallNeededLibraries()
        {
            var messageBox = new MessageBox();
            messageBox.ButtonRightName = "I Understand";
            messageBox.ButtonLeftClick += CloseMessageBoxEventHandler;
            messageBox.ButtonRightClick += CloseMessageBoxEventHandler;

            messageBox.Show("Error", "We could not install needed libraries. Please contact support.");

            _navigationService.Navigate(typeof(ServerBrowserPage));
        }

        private void ShowCantCreateDownloadDirectoryPopup()
        {
            MessageBox messageBox = new MessageBox();

            messageBox.ButtonRightName = "I Understand";
            messageBox.ButtonLeftClick += CloseMessageBoxEventHandler;
            messageBox.ButtonRightClick += CloseMessageBoxEventHandler;

            messageBox.Show("Error", "We could not create download directory. Please contact support.");

            _navigationService.Navigate(typeof(ServerBrowserPage));
        }

        private void ShowCantEnsureLibraryIsUpToDate(string libraryFilePath)
        {
            MessageBox messageBox = new MessageBox();

            messageBox.ButtonRightName = "I Understand";
            messageBox.ButtonLeftClick += CloseMessageBoxEventHandler;
            messageBox.ButtonRightClick += CloseMessageBoxEventHandler;
            messageBox.SizeToContent = System.Windows.SizeToContent.WidthAndHeight;

            messageBox.Show("Error", $"We could not check updates.\nPlease contact support.\nSupport details: '{libraryFilePath}'");

            _navigationService.Navigate(typeof(ServerBrowserPage));
        }

        // Function delegate (allows passing function as parameter)
        delegate Task<byte[]> DownloadDelegate();

        private async Task<bool> EnsureLibraryIsUpToDate(string libraryFilePath, byte[] libraryLatestHash, DownloadDelegate libraryDownloadFunction)
        {
            try
            {
                // Is up to date if file exists and library is up-to-date (checks hash)
                if (File.Exists(libraryFilePath) && IsLocalLibraryUpToDate(libraryFilePath, libraryLatestHash))
                    return true;

                // Otherwise
                // Retrieve the updated library as bytes
                byte[] latestLibraryBytes = await libraryDownloadFunction();

                // Writes the updated library on the expected path
                await File.WriteAllBytesAsync(libraryFilePath, latestLibraryBytes);

                // Updated library
                return true;

            }
            catch (Exception exception)
            {
                _logger.LogError($"Exception happened: {exception.Message}");
                ShowCantEnsureLibraryIsUpToDate(libraryFilePath);
            }
            return false;
        }


        private string GetSteamInstallationDirectoryPath()
        {
            // Build registry path to get steam installation directory
            string steamPath = @"Software\" + ((System.Environment.Is64BitOperatingSystem) ? @"Wow6432Node\" : string.Empty) + @"Valve\Steam";

            // Search on the registry
            RegistryKey? registryKey;
            try
            {
                registryKey = Registry.LocalMachine.OpenSubKey(steamPath);

                if (registryKey is null)
                    return string.Empty;

                string installPath = "InstallPath";

                var registryValueKind = registryKey.GetValueKind(installPath);

                if (registryValueKind != RegistryValueKind.String) // Has to be REG_SZ
                    return string.Empty;

                object? registryValue = registryKey.GetValue(installPath); // Read the value

                if (registryValue is null)
                    return string.Empty;

                if (registryValue is not string)
                    return string.Empty;

                return (string)registryValue; // We found the value
            }
            catch (Exception exception)
            {
                _logger.LogError(exception.Message);
                return string.Empty;
            }
        }

        private bool IsLocalLibraryUpToDate(string libraryFilePath, byte[] libraryExpectedHash)
        {
            try
            {
                // Create a SHA512 computing object
                SHA512 sha512 = SHA512.Create();

                // Open a stream from the file
                FileStream localLibraryFileStream = File.OpenRead(libraryFilePath);

                // Compute the hash from the stream (computes the file)
                byte[] localLibraryHash = sha512.ComputeHash(localLibraryFileStream);

                localLibraryFileStream.Close();

                // Returns true if is up to date (expected hash equals local hash), otherwise false
                return libraryExpectedHash.SequenceEqual(localLibraryHash);
            }
            catch (Exception exception) // Capture the exception
            {
                _logger.LogError(exception.Message);
            }
            return false;
        }

        private void CloseMessageBoxEventHandler(object sender, System.Windows.RoutedEventArgs e)
        {
            (sender as MessageBox)?.Close();
        }
        private void LockedPopup_ButtonLeftClick(object sender, System.Windows.RoutedEventArgs e)
        {
            _clientExecutionService.ForceUnlockLauncher();

            _dialogControl.ButtonRightClick -= LockedPopup_ButtonLeftClick;

            (sender as MessageBox)?.Close();
        }


        public void CloseDialogEventHandler()
        {
            _dialogControl.Hide();
        }


    }
}
