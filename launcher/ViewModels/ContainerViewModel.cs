using launcher.Services;
using Microsoft.Toolkit.Mvvm.ComponentModel;
using System;
using System.Collections.ObjectModel;
using Wpf.Ui.Common;
using Wpf.Ui.Controls;
using Wpf.Ui.Controls.Interfaces;
using Wpf.Ui.Mvvm.Contracts;

namespace launcher.ViewModels
{
    public partial class ContainerViewModel : ObservableObject
    {
        private readonly IRepositoryService _repositoryService;
        private bool _isInitialized = false;

        [ObservableProperty]
        private string _applicationTitle = String.Empty;

        [ObservableProperty]
        private ObservableCollection<INavigationControl> _navigationItems = new();

        [ObservableProperty]
        private ObservableCollection<INavigationControl> _navigationFooter = new();

        public ContainerViewModel(INavigationService navigationService, IRepositoryService repositoryService)
        {
            _repositoryService = repositoryService;

            if (!_isInitialized)
                InitializeViewModel();
        }
        
        private void InitializeViewModel()
        {

            ApplicationTitle = "Just Cause: 1 Multiplayer";

            NavigationItems = new ObservableCollection<INavigationControl>
            {
                new NavigationItem()
                {
                    Content = "Home",
                    PageTag = "dashboard",
                    Icon = SymbolRegular.Home24,
                    PageType = typeof(Views.Pages.DashboardPage)
                },
                new NavigationItem()
                {
                    Content = "Servers",
                    PageTag = "servers",
                    Icon = SymbolRegular.CloudFlow24,
                    PageType = typeof(Views.Pages.ServerBrowserPage)
                },
                new NavigationItem()
                {
                    Content = "Game settings",
                    PageTag = "game_settings",
                    Icon = SymbolRegular.AppsListDetail20,
                    PageType = typeof(Views.Pages.GameSettingsPage)
                }
            };

            NavigationFooter = new ObservableCollection<INavigationControl>
            {
                new NavigationItem()
                {
                    Content = "Settings",
                    PageTag = "settings",
                    Icon = SymbolRegular.Settings24,
                    PageType = typeof(Views.Pages.SettingsPage)
                }
            };

            _isInitialized = true;
        }
    }
}
