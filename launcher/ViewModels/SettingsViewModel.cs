using launcher.Models;
using launcher.Services;
using Microsoft.Toolkit.Mvvm.ComponentModel;
using Microsoft.Toolkit.Mvvm.Input;
using System;
using System.Threading.Tasks;
using System.Windows.Input;
using Wpf.Ui.Common.Interfaces;

namespace launcher.ViewModels
{
    public partial class SettingsViewModel : ObservableObject, INavigationAware
    {
        private readonly IRepositoryService _repositoryService;
        private bool _isInitialized = false;

        [ObservableProperty]
        private string _appVersion = String.Empty;

        private LauncherSettingTheme _currentTheme = LauncherSettingTheme.Dark;
        public LauncherSettingTheme CurrentTheme
        {
            set
            {
                SetProperty(ref _currentTheme, value);
                switch (_currentTheme)
                {
                    case LauncherSettingTheme.Light:
                        Wpf.Ui.Appearance.Theme.Apply(Wpf.Ui.Appearance.ThemeType.Light);
                        break;
                    case LauncherSettingTheme.Dark:
                        Wpf.Ui.Appearance.Theme.Apply(Wpf.Ui.Appearance.ThemeType.Dark);
                        break;
                    case LauncherSettingTheme.HighContrast:
                        Wpf.Ui.Appearance.Theme.Apply(Wpf.Ui.Appearance.ThemeType.HighContrast);
                        break;
                    default:
                        Wpf.Ui.Appearance.Theme.Apply(Wpf.Ui.Appearance.ThemeType.Dark);
                        break;

                }

                Properties.Settings.Default.Theme = (int)_currentTheme;
            }
            get
            {
                return _currentTheme;
            }
        }


        public SettingsViewModel(IRepositoryService repositoryService)
        {
            _repositoryService = repositoryService;
        }

        public void OnNavigatedTo()
        {
            if (!_isInitialized)
            {
                InitializeViewModel();

                // Restore from properties.
                //
                CurrentTheme = (LauncherSettingTheme)Properties.Settings.Default.Theme;

                _isInitialized = true;
            }
        }

        public void OnNavigatedFrom()
        {
            // View model servers as a cache service to not complicate things.
            // Therefore, flush to the repository when dismissing the view.
            // This way we don't have to prompt or add buttons to the user
            // and make whole UX a little bit better and clean.
            //
            if (!_isInitialized) return;

        }

        private void InitializeViewModel()
        {
            AppVersion = "Version: " + GetAssemblyVersion();
            _isInitialized = true;
        }

        private string GetAssemblyVersion()
        {
            return System.Reflection.Assembly.GetExecutingAssembly().GetName().Version?.ToString() ?? String.Empty;
        }
    }
}
