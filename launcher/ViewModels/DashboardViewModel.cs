using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Microsoft.Toolkit.Mvvm.ComponentModel;
using Microsoft.Toolkit.Mvvm.Input;
using Wpf.Ui.Common.Interfaces;
using Wpf.Ui.Controls.Interfaces;
using Wpf.Ui.Mvvm.Contracts;

using launcher.Models;
using launcher.Services;

namespace launcher.ViewModels
{
    public partial class DashboardViewModel : ObservableObject, INavigationAware
    {
        // Dependency Injections.
        //
        private readonly INavigationService _navigationService;
        private readonly IDialogControl _dialogControl;
        private readonly IRepositoryService _repositoryService;

        // Observables
        //
        [ObservableProperty]
        private IEnumerable<DashboardNew>? _news;

        // Private
        //
        private bool _isInitialized = false;

        public DashboardViewModel(INavigationService navigationService, IDialogService dialogService, IRepositoryService repositoryService)
        {
            _isInitialized = false;
            _navigationService = navigationService;
            _dialogControl = dialogService.GetDialogControl();
            _repositoryService = repositoryService;
        }
        public void OnNavigatedTo()
        {
            if (!_isInitialized)
                InitializeViewModel();
        }

        public void OnNavigatedFrom()
        {
        }

        private void InitializeViewModel()
        {
            // Construct news. Static content for now, otherwise we could make it asynchronous and show progress.
            // We have news currently in the view, since its static content. This is just a placeholder for future
            // if we decide to pull dynamic content from the backend or any other repository.
            //
            News = new ObservableCollection<DashboardNew>
            {
                new DashboardNew
                {
                    Title = "(Un)official Launcher released",
                    Description = "Now, more than ever, you can simply press play and enjoy the game with your " +
                    "friends on the fields of San Esperito. Enjoy and have fun.",
                    Date = new DateTime(2022, 12, 16)
                },
                new DashboardNew
                {
                    Title = "New server and features",
                    Description = "We have added first dedicated server and added bunch of new in-game features " +
                    "for the most immersing JC1:MP experience. Join us!",
                    Date = new DateTime(2022, 09, 11)
                },
                new DashboardNew
                {
                    Title = "Public Discord announcement",
                    Description = "We are proud to reach broader public and core fans who wants to play on our " +
                    "multiplayer mod. Join us on: https://discord.gg/dq33yEyjv1",
                    Date = new DateTime(2022, 07, 17)
                }
            };
            _isInitialized = true;
        }

        // Relays.
        //
        [ICommand]
        private async void OnPlayNow()
        {
            var result = await _dialogControl.ShowAndWaitAsync("Information", "Bugs might occur and user experience might be hindered. We ask you to stay positive and provide necessary feedback to improve the service. Thank you for joining us. Enjoy!");
            if(result == IDialogControl.ButtonPressed.Left)
            {
                // If client already configured their game, move directly to server browser,
                // otherwise point them to game configuration and settings page.
                //
                if(await _repositoryService.IsInitializedAsync())
                { 
                    _navigationService.Navigate(typeof(Views.Pages.ServerBrowserPage)); 
                }
                else
                {
                    _navigationService.Navigate(typeof(Views.Pages.GameSettingsPage));
                }
                
            }
            var dialogControl = _dialogControl;
            dialogControl.Hide();

        }
    }
}
