using launcher.Services.Connection;
using Wpf.Ui.Common.Interfaces;

namespace launcher.Views.Pages
{
    /// <summary>
    /// Interaction logic for DataView.xaml
    /// </summary>
    public partial class ServerBrowserPage : INavigableView<ViewModels.ServerBrowserViewModel>
    {
        public ViewModels.ServerBrowserViewModel ViewModel
        {
            get;
        }

        public ServerBrowserPage(ViewModels.ServerBrowserViewModel viewModel, IServerService serverService)
        {
            ViewModel = viewModel;

            InitializeComponent();
        }
    }
}
