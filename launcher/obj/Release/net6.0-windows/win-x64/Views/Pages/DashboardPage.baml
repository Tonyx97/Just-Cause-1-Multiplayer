   M S B A M L   `   `   `  ���� C  ?launcher, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null;"clr-namespace:launcher.Views.Pageslauncher.Views.Pages  (    "launcher.Views.Pages.DashboardPage   W SPresentationCore, Version=6.0.2.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=6.0.2.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35e aSystem.Windows.Controls.Ribbon, Version=6.0.2.0, Culture=neutral, PublicKeyToken=b77a5c561934e089R NWindowsBase, Version=6.0.2.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     R NSystem.Xaml, Version=6.0.2.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     @d2http://schemas.microsoft.com/expression/blend/2008     4local"clr-namespace:launcher.Views.Pages     Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     A =Wpf.Ui, Version=2.0.2.0, Culture=neutral, PublicKeyToken=null/ui&http://schemas.lepo.co/wpfui/2022/xaml    N� Title$  DashboardPage��   TextFillColorPrimaryBrush N� 
Foreground# �   .��$� -    $�Name$ 	RootPanel��$��
18,0,18,18q� %� IsVirtualizing$
 True�� %� VirtualizationMode$ 	Recycling=�.+��  � RowDefinitions �� $
��Auto ��� $
��Auto �.+��� $��0��$	��400�� �� CornerRadius$ 8,0,0,0������  �� ImageSource$- 'pack://application:,,,/assets/cover.png�� j� Stretch$ UniformToFill=�._��� $ 8,0,0,0����
� .��� 	 � Offset$	 0Q�
 � Color$
 	#2F000000��� $	 1Q�$
 	#8F000000��._��� $��16q�$
��Left=�$	��Top=�
������ .+��� $��18(�
�������$��
Welcome to���� $��28(�$
��Bold"�
�������$��Just Cause: Multiplayer���� $��18(� J� Opacity$ .8Q�
�������$.��(You play as Rico, but with your friends.��  Wpf.Ui.Controls.Button  $��36��$��0,6,0,0q�$
��30,6q�$
��Left=�   
Appearance$ Transparent=�$��Play now�� " TextFillColorLightPrimaryBrush#���  ���� *ViewModel.OnPlayNowCommand+ �� Mode$ OneWay=�� $��1��$��16q� �� $��* ��� $��* �.+��� $��0��$��16(�$
��Bold"�$��What is new��� $��1��$��0,12,0,0q� � IsSharedSizeScope$
 True�� � ColumnDefinitions �� $��* ��� $��* ��� $��* �.+�  Wpf.Ui.Controls.Card  $��0��$��0,0,4,0q�.���� .+��� $��0��$��16(�$
��Bold"�$$��(Un)official Launcher released���� $	��250��$
��Left=�$��0��$��12(�  TextFillColorTertiaryBrush#���  $��WrapWithOverflow=�$����Now, more than ever, you can simply press play and enjoy the game with your friends on the fields of San Esperito. Enjoy and have fun.���� $��0, 12, 0, 0q�$��0��$��12(�$
��Bold"�$��29. 10. 2022��  $��1��$��4,0,4,0q�.���� .+��� $��0��$��16(�$
��Bold"�$��New server and features���� $	��250��$
��Left=�$��0��$��12(�#���  $��WrapWithOverflow=�$���We have added first dedicated server and added bunch of new in-game features for the most immersing JC1:MP experience. Join us!���� $��0, 12, 0, 0q�$��0��$��12(�$
��Bold"�$��11. 09. 2022��  $��2��$��4,0,4,0q�.���� .+��� $��0��$��16(�$
��Bold"�$!��Public Discord announcement���� $	��250��$
��Left=�$��0��$��12(�#���  $��WrapWithOverflow=�$����We are proud to reach broader public and core fans who wants to play on our multiplayer mod. Join us on: https://discord.gg/dq33yEyjv1���� $��0, 12, 0, 0q�$��0��$��12(�$
��Bold"�$��17. 07. 2022��