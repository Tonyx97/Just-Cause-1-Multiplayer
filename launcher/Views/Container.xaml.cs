using System;
using System.Windows;
using System.Windows.Controls;
using Wpf.Ui.Controls.Interfaces;
using Wpf.Ui.Mvvm.Contracts;

namespace launcher.Views
{
    /// <summary>
    /// Interaction logic for Container.xaml
    /// </summary>
    public partial class Container : INavigationWindow
    {
        public ViewModels.ContainerViewModel ViewModel
        {
            get;
        }

        public Container(ViewModels.ContainerViewModel viewModel, IPageService pageService, INavigationService navigationService, IDialogService dialogService)
        {
            ViewModel = viewModel;
            DataContext = this;

            // Fetch Launcher
            //
            Height = Properties.Settings.Default.WindowHeight;
            Width = Properties.Settings.Default.WindowWidth;
            Top = Properties.Settings.Default.WindowTop;
            Left = Properties.Settings.Default.WindowLeft;
            Wpf.Ui.Appearance.Theme.Apply((Wpf.Ui.Appearance.ThemeType)(Properties.Settings.Default.Theme + 1));

            InitializeComponent();
            SetPageService(pageService);


            // Allows you to use the Dialog control defined in this window in other pages or windows
            dialogService.SetDialogControl(RootDialog);

            navigationService.SetNavigationControl(RootNavigation);
        }

        #region INavigationWindow methods

        public Frame GetFrame()
            => RootFrame;

        public INavigation GetNavigation()
            => RootNavigation;

        public bool Navigate(Type pageType)
            => RootNavigation.Navigate(pageType);

        public void SetPageService(IPageService pageService)
            => RootNavigation.PageService = pageService;

        public void ShowWindow()
            => Show();

        public void CloseWindow()
            => Close();

        #endregion INavigationWindow methods

        /// <summary>
        /// Raises the closed event.
        /// </summary>
        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);

            // Cache window frame.
            //
            Properties.Settings.Default.WindowHeight = Height;
            Properties.Settings.Default.WindowWidth = Width;
            Properties.Settings.Default.WindowTop = Top;
            Properties.Settings.Default.WindowLeft = Left;

            // Save all properties globally.
            //
            Properties.Settings.Default.Save();

            // Make sure that closing this window will begin the process of closing the application.
            Application.Current.Shutdown();
        }
    }
}