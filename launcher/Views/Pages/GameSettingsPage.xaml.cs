using System.Diagnostics;
using System.IO;
using Wpf.Ui.Common.Interfaces;
using Wpf.Ui.Mvvm.Contracts;

namespace launcher.Views.Pages
{
    /// <summary>
    /// Interaction logic for GameSettingsPage.xaml
    /// </summary>
    public partial class GameSettingsPage : INavigableView<ViewModels.GameSettingsViewModel>
    {
        public ViewModels.GameSettingsViewModel ViewModel
        {
            get;
        }

        public GameSettingsPage(ViewModels.GameSettingsViewModel viewModel, ISnackbarService snackbarService)
        {
            ViewModel = viewModel;

            InitializeComponent();
            snackbarService.SetSnackbarControl(SnackBarFirstTime);
        }

        private void TextBox_PreviewMouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            ViewModel.SearchGamePathCommand.Execute(sender);
        }

        private void OpenGameDirectoryEventHandler(object sender, System.Windows.RoutedEventArgs e)
        {
            string? gameExecutablePath = ViewModel.GamePath;

            if (string.IsNullOrEmpty(gameExecutablePath))
                return;

            string? gameDirectoryPath = Path.GetDirectoryName(gameExecutablePath);

            if (string.IsNullOrEmpty(gameDirectoryPath))
                return;

            Process.Start("explorer.exe", gameDirectoryPath);
        }
    }
}
