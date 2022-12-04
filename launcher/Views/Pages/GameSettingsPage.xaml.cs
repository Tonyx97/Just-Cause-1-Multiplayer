using Wpf.Ui.Common.Interfaces;
using Wpf.Ui.Mvvm.Contracts;
using Wpf.Ui.Mvvm.Interfaces;

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
    }
}
