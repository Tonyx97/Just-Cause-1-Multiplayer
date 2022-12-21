using launcher.ViewModels;
using Wpf.Ui.Common.Interfaces;

namespace launcher.Views.Pages
{
    /// <summary>
    /// Interaction logic for DataView.xaml
    /// </summary>
    public partial class ServerBrowserPage : INavigableView<ServerBrowserViewModel>
    {
        public ServerBrowserViewModel ViewModel { get; }

        public ServerBrowserPage(ServerBrowserViewModel viewModel)
        {
            ViewModel = viewModel;
            InitializeComponent();
        }
    }
}
