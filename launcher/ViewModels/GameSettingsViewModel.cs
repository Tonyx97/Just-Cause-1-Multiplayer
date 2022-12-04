using launcher.Models;
using launcher.Services;
using Microsoft.Toolkit.Mvvm.ComponentModel;
using Microsoft.Toolkit.Mvvm.Input;
using Microsoft.Win32;
using System.IO;
using System.Threading.Tasks;
using Wpf.Ui.Common.Interfaces;
using Wpf.Ui.Mvvm.Contracts;

namespace launcher.ViewModels
{
    public partial class GameSettingsViewModel : ObservableObject, INavigationAware
    {
        // Dependency Injections.
        //
        private readonly ISnackbarService _snackbarService;
        private readonly IRepositoryService _repositoryService;

        // Observables
        //
        [ObservableProperty]
        private string? _gamePath = "";
        [ObservableProperty]
        private string? _username = "";
        [ObservableProperty]
        private bool _motionBlur = false;
        [ObservableProperty]
        private bool _heatHaze = false;
        [ObservableProperty]
        private bool _postFx = false;
        [ObservableProperty]
        private GameSettingTextureResolution _textureResolution;
        [ObservableProperty]
        private GameSettingSceneComplexity _sceneComplexity;
        [ObservableProperty]
        private GameSettingWaterQuality _waterQuality;
        [ObservableProperty]
        private int _fxVolume;

        // Private
        //
        private bool _isInitialized = false;

        public GameSettingsViewModel(ISnackbarService snackbarService, IRepositoryService repositoryService)
        {
            _isInitialized = false;
            _snackbarService = snackbarService;
            _repositoryService = repositoryService;
        }

        public async void OnNavigatedTo()
        {
            if (!_isInitialized)
            {
                await InitializeViewModel();

                _isInitialized = true;
            }
        }

        public async void OnNavigatedFrom()
        {
            // View model servers as a cache service to not complicate things.
            // Therefore, flush to the repository when dismissing the view.
            // This way we don't have to prompt or add buttons to the user
            // and make whole UX a little bit better and clean.
            //
            if (!_isInitialized) return;

            var settings = new GameSettings();
            settings.GamePath = _gamePath;
            settings.Username = _username;
            settings.MotionBlur = _motionBlur;
            settings.HeatHaze = _heatHaze;
            settings.PostFx = _postFx;
            settings.TextureResolution = _textureResolution;
            settings.SceneComplexity = _sceneComplexity;
            settings.WaterQuality = _waterQuality;
            settings.FxVolume = _fxVolume;

            var result = await _repositoryService.InsertOrUpdateData(settings);
            if (result == false)
            {
                PromptErrorMessageBox();
            }

        }

        private async Task InitializeViewModel()
        {
            // If client did not configured/initialized the game config yet, display
            // snackbar to notify them about navigation snap.
            //
            if (await _repositoryService.IsInitializedAsync() == false)
            {
                // Escape on purpose to continue executing on initialization.
                //
                _snackbarService.Show();

                // Create and populate default.
                //
                var isInitialized = await _repositoryService.InitializeAsync();
                if (isInitialized == false)
                {
                    PromptErrorMessageBox();
                    return;
                }

                Username = "";
                GamePath = "";
                MotionBlur = false;
                HeatHaze = true;
                PostFx = true;
                TextureResolution = GameSettingTextureResolution.High;
                SceneComplexity = GameSettingSceneComplexity.High;
                WaterQuality = GameSettingWaterQuality.High;
                FxVolume = 4; // 0 - 10.

                var settings = new GameSettings();
                settings.GamePath = _gamePath;
                settings.Username = _username;
                settings.MotionBlur = _motionBlur;
                settings.HeatHaze = _heatHaze;
                settings.PostFx = _postFx;
                settings.TextureResolution = _textureResolution;
                settings.SceneComplexity = _sceneComplexity;
                settings.WaterQuality = _waterQuality;
                settings.FxVolume = _fxVolume;

                var result = await _repositoryService.InsertOrUpdateData(settings);
                if (result == false)
                {
                    PromptErrorMessageBox();
                }
            }
            else
            {
                // Populate data from the registry.
                //
                var result = await _repositoryService.FetchDataAsync();
                if (result != null)
                {
                    GamePath = result.GamePath;
                    Username = result.Username;
                    MotionBlur = result.MotionBlur;
                    HeatHaze = result.HeatHaze;
                    PostFx = result.PostFx;
                    TextureResolution = result.TextureResolution;
                    SceneComplexity = result.SceneComplexity;
                    WaterQuality = result.WaterQuality;
                    FxVolume = result.FxVolume; // 0 - 10.
                }
                else
                {
                    PromptErrorMessageBox();
                    return;
                }
            }

            // Try to fetch game path from known locations. Dispatch to background.
            //
            if (GamePath?.Length == 0)
            {
                await Task.Factory.StartNew(() =>
                {
                    // This will only work if client has game dir pointed to the same directory
                    // as steam installation aka, if they didn't change game directory on install.
                    //
                    var steamKey = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Valve\\Steam";
                    var steamDirectory = (string)Registry.GetValue(steamKey, "InstallPath", null);
                    if (steamDirectory != null)
                    {
                        var steamAppsDirectory = Path.Combine(steamDirectory, "steamapps");
                        if (Directory.Exists(steamAppsDirectory))
                        {
                            var steamCommonDirectory = Path.Combine(steamAppsDirectory, "common");
                            var justCauseDirectory = Path.Combine(steamCommonDirectory, "Just Cause");
                            var justCausePath = Path.Combine(justCauseDirectory, "JustCause.exe");
                            if (File.Exists(justCausePath))
                            {
                                // We got it, dispatch to main thread.
                                //
                                GamePath = justCausePath;
                                return;
                            }
                        }
                    }

                    // Brute force two most common installation directories.
                    //
                    var possiblePath = Path.Combine("C:");
                    possiblePath = Path.Combine(possiblePath, "Program Files (x86)");
                    possiblePath = Path.Combine(possiblePath, "Steam");
                    possiblePath = Path.Combine(possiblePath, "steamapps");
                    possiblePath = Path.Combine(possiblePath, "common");
                    possiblePath = Path.Combine(possiblePath, "Just Cause");
                    possiblePath = Path.Combine(possiblePath, "JustCause.exe");
                    if (File.Exists(possiblePath))
                    {
                        // We got it, dispatch to main thread.
                        //
                        GamePath = possiblePath;
                        return;
                    }

                    possiblePath = "";
                    possiblePath = Path.Combine("D:");
                    possiblePath = Path.Combine(possiblePath, "SteamLibrary");
                    possiblePath = Path.Combine(possiblePath, "steamapps");
                    possiblePath = Path.Combine(possiblePath, "common");
                    possiblePath = Path.Combine(possiblePath, "Just Cause");
                    possiblePath = Path.Combine(possiblePath, "JustCause.exe");
                    if (File.Exists(possiblePath))
                    {
                        // We got it, dispatch to main thread.
                        //
                        GamePath = possiblePath;
                        return;
                    }
                });
            }
        }

        private void MessageBox_LeftButtonClick(object sender, System.Windows.RoutedEventArgs e)
        {
            (sender as Wpf.Ui.Controls.MessageBox)?.Close();
        }

        private void PromptErrorMessageBox()
        {
            var messageBox = new Wpf.Ui.Controls.MessageBox();

            messageBox.ButtonLeftName = "Confirm";

            messageBox.ButtonLeftClick += MessageBox_LeftButtonClick;

            messageBox.Show("Error", "Error occurred. Try to run Launcher as an admin instead (this is debug message, feel free to report it as a feedback).");
        }

        [ICommand]
        private void SearchGamePath(object? textBox)
        {
            if (textBox is Wpf.Ui.Controls.TextBox)
            {
                // Open legacy file dialog.
                //
                var dialog = new Microsoft.Win32.OpenFileDialog();
                dialog.FileName = "JustCause";
                dialog.DefaultExt = ".exe";
                dialog.Filter = "Executable files (.exe)|*.exe";

                // Show open file dialog box
                bool? result = dialog.ShowDialog();

                // Process open file dialog box results
                if (result == true)
                {
                    // Open document
                    GamePath = dialog.FileName;
                }
            }
        }
    }
}
