   M S B A M L   `   `   `  ���� C  ?launcher, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null/clr-namespace:launcher.Viewslauncher.Views  ;"clr-namespace:launcher.Views.Pageslauncher.Views.Pages      launcher.Views.Container   W SPresentationCore, Version=6.0.2.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=6.0.2.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35e aSystem.Windows.Controls.Ribbon, Version=6.0.2.0, Culture=neutral, PublicKeyToken=b77a5c561934e089R NWindowsBase, Version=6.0.2.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     R NSystem.Xaml, Version=6.0.2.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     @d2http://schemas.microsoft.com/expression/blend/2008     .localclr-namespace:launcher.Views     Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     4pages"clr-namespace:launcher.Views.Pages     A =Wpf.Ui, Version=2.0.2.0, Culture=neutral, PublicKeyToken=null/ui&http://schemas.lepo.co/wpfui/2022/xaml  $
��1190��$	��760��  Wpf.Ui.Controls.UiWindow!    ExtendsContentIntoTitleBar$
  True��   WindowBackdropType$
 Mica=�   WindowCornerPreference$ Round=� � Title �� *ViewModel.ApplicationTitle+ �� Mode$ OneWay=�.���  � ColumnDefinitions �� $
��Auto ��� $��* �.+�'  !Wpf.Ui.Controls.NavigationCompact  -     Name$ RootNavigation��$��0��/  )Wpf.Ui.Controls.Navigation.NavigationBase   Footer �� *ViewModel.NavigationFooter+$ TwoWay=�   Items �� *ViewModel.NavigationItems+$ TwoWay=�	   Frame	 �� 
 �� ElementName$
 	RootFrame��$ OneWay=�  Wpf.Ui.Controls.TitleBar  $��0��$��0��$��2��$��42,0,0,0q�   Icon$, &pack://application:,,,/assets/icon.png��   Title �� *ViewModel.ApplicationTitle+$ OneWay=�� $��1��.+��� $��0,46,0,0q�    ControlFillColorDefaultBrush#���    �� CornerRadius$ 8,0,0,0��._��  � RowDefinitions �� $
��Auto ��� $��* �.+� � -     �Name$ 	RootFrame��$��1��   Wpf.Ui.Controls.Breadcrumb  $��0��$��56, 32q�$
��Left=�$	��Top=�$��24(�   
Navigation �� $
 RootNavigation��$ OneWay=�  Wpf.Ui.Controls.Snackbar  -     Name$ RootSnackbar��$��2��$��Bottom=�$��0��$	��600��   
Appearance$ 	Secondary=�   CloseButtonEnabled$
 True��   Icon$ PuzzlePiece24=�   Timeout$
 5000��  Wpf.Ui.Controls.Dialog  -     Name$ 
RootDialog��$��2��$��2��$��0��   ButtonLeftName$ I understand��