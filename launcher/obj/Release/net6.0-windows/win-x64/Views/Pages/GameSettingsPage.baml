   M S B A M L   `   `   `  ���� C  ?launcher, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null3clr-namespace:launcher.Helperslauncher.Helpers  O Kmscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e0891&clr-namespace:System;assembly=mscorlibSystem ;"clr-namespace:launcher.Views.Pageslauncher.Views.Pages  1clr-namespace:launcher.Modelslauncher.Models  +    %launcher.Views.Pages.GameSettingsPage   W SPresentationCore, Version=6.0.2.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=6.0.2.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35e aSystem.Windows.Controls.Ribbon, Version=6.0.2.0, Culture=neutral, PublicKeyToken=b77a5c561934e089R NWindowsBase, Version=6.0.2.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     R NSystem.Xaml, Version=6.0.2.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     @d2http://schemas.microsoft.com/expression/blend/2008     2helpersclr-namespace:launcher.Helpers     6sys&clr-namespace:System;assembly=mscorlib     4local"clr-namespace:launcher.Views.Pages     0modelsclr-namespace:launcher.Models     Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     A =Wpf.Ui, Version=2.0.2.0, Culture=neutral, PublicKeyToken=null/ui&http://schemas.lepo.co/wpfui/2022/xaml     TextFillColorPrimaryBrush  N� 
Foreground#  �    N� Title$ GameSettingsPage��  Wpf.Ui.Controls.UiPage   
Scrollable$
 True�� � 	Resources   TextureResolutionDataSource S� 
ObjectTypeU QSystem.Runtime, Version=6.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a  System.Enum S� 
MethodName S� MethodParameters M� TypeName  SceneComplexityDataSource  WaterQualityDataSource��%�   &	       &	 M     &	 �     S� "  $ 	GetValues�� M� $) #models:GameSettingTextureResolution��S� "  $ 	GetValues�� M� $' !models:GameSettingSceneComplexity��S� "  $ 	GetValues�� M� $$ models:GameSettingWaterQuality��.��� $��56, 0q� � RowDefinitions �� $
��Auto ��� $
��Auto ��� $
��Auto �.+��� $��0��$��0, 0, 0, 12q�.+��� $��20(�$��Medium"�$��General��"  Wpf.Ui.Controls.CardExpander  $��12, 6, 0, 0q�	   Icon$	 Person48=�$
{�True��
 9� 
IsExpanded$
 False�����  � ColumnDefinitions �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��Username���� $��12(�  TextFillColorTertiaryBrush#���  $(��"Your username. Field is mandatory.��  Wpf.Ui.Controls.TextBox  $��1��$��0,0,16,0q�$	��200��$	��500��   PlaceholderText$ Your username here������ *ViewModel.Username+.���� $����You have all the freedom to pick whatever nickname you want for yourself. There are no limitations. This is how other people will see you and refer to you at our multiplayer adventure. Choose wisely.�� �� TextAlignment$
 Left=�$
��Left=�$
��Wrap=�  $��12, 6, 0, 0q�$	 Folder48=�$
{�True��$
 False�����  �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��	Game path���� $��12(�#���  $0��*Path to JustCause.exe. Field is mandatory.��  -   $��0,0,16,0q�$��1��$	��200��$	��460��$! Click to browse file system�� �� 
IsReadOnly$
 True������ *ViewModel.GamePath+.��  Wpf.Ui.Controls.Button  -   $��0,0,20,0q�$��Right=�$��Center=�$��4,4,4,4q�.��Open in Explorer.���� $����To automate the process further, in order to install and prepare multiplayer mod requirements, we need to know where Just Cause: 1 is located. We will try to automatically detect installation directory of the game. Sometimes our approach might fail and manually pinning is necessary. Please browse to the JustCause.exe by clicking on the text box.��$
 Left=�$
��Left=�$
��Wrap=��� $��1��$��0, 0, 0, 12q�.+��� $��20(�$��Medium"�$��Graphics��  $��12, 12, 0, 0q�$	 Blur20=�$
{�True��$
 False�����  �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��Motion Blur���� $��12(�#���  $+��%Enable or disable engine motion blur.��"  Wpf.Ui.Controls.ToggleSwitch  $��1��$��0,0,16,0q� d� 	IsChecked �� *ViewModel.MotionBlur+ �� Mode$ TwoWay=�.���� $����Motion blur is the visual streaking or smearing captured on camera as a result of movement of the camera, the subject, or a combination of the two. The human eye naturally registers motion blur in real life, so a subtle blur in film often mimics reality better than if it were absent. When applied intentionally, this effect may be referred to as aesthetic blur.��$
 Left=�$
��Left=�$
��Wrap=�  $��12, 6, 0, 0q�$	 WeatherSunnyLow48=�$
{�True��$
 False�����  �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��	Heat Haze���� $��12(�#���  $)��#Enable or disable engine heat haze.��  $��1��$��0,0,16,0q� �� *ViewModel.HeatHaze+$ TwoWay=�.���� $����Heat haze is an atmospheric condition that occurs when a body such as the ground is reflecting a lot of heat. It is caused by the difference in temperature between the hot body and cooler air around it.��$
 Left=�$
��Left=�$
��Wrap=�  $��12, 6, 0, 0q�$	 VideoBackgroundEffect20=�$
{�True��$
 False�����  �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��Post FX���� $��12(�#���  $'��!Enable or disable engine Post FX.��  $��1��$��0,0,16,0q� �� *ViewModel.PostFx+$ TwoWay=�.���� $����Post-FX nodes are mostly effects added onto an image, after the scene is rendered. These can vary from Colour Correction to FXAA antialiasing, and are generally used to enhance an image.��$
 Left=�$
��Left=�$
��Wrap=�  $��12, 6, 0, 0q�$	 
CodeText20=�$
{�True��$
 False�����  �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��Texture Resolution���� $��12(�#���  $*��$Select desirable texture resolution.���� $��1��$��0,0,16,0q�$	��200������  �� Source# [  �� SelectedItem �� *ViewModel.TextureResolution+.���� $����Texture quality is generally representative of the actual resolution (in pixels) of textures mapped to objects. Higher resolution textures result in significantly deeper details, but are also abusive on VRAM and video processing hardware.��$
 Left=�$
��Left=�$
��Wrap=�  $��12, 6, 0, 0q�$	 Image20=�$
{�True��$
 False�����  �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��Scene Complexity���� $��12(�#���  $(��"Select desirable scene complexity.���� $��1��$��0,0,16,0q�$	��200������ # [  �� *ViewModel.SceneComplexity+.���� $����Scene Complexity controls the in-game representation of how detailed objects are. A higher setting here results in more complex geometry in things like foliage, rocks, as well as making objects remain highly detailed at farther distances from the player.��$
 Left=�$
��Left=�$
��Wrap=�  $��12, 6, 0, 0q�$	 DataWaterfall20=�$
{�True��$
 False�����  �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��Water Quality���� $��12(�#���  $%��Select desirable water quality.���� $��1��$��0,0,16,0q�$	��200������ # [  �� *ViewModel.WaterQuality+.���� $M��GAdditional texture and rendering characteristic for water in the world.��$
 Left=�$
��Left=�$
��Wrap=��� $��2��$��0, 0, 0, 12q�.+��� $��20(�$��Medium"�$��Audio��  $��12, 12, 0, 0q�$	 
Speaker016=�$
{�True��$
 False�����  �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��Music Volume���� $��12(�#���  $��Adjust music volume.���� $��1��$��0,8,16,0q�$	��200�� � Maximum$ 10Q� � Minimum$ 0Q� �� TickFrequency$ 1Q� � Value �� *ViewModel.Volume+.���� $��Game music volume��$
 Left=�$
��Left=�$
��Wrap=�  $��12, 12, 0, 0q�$	 TabletSpeaker20=�$
{�True��$
 False�����  �� $��* ��� $
��Auto �.+��� $��0��$��Center=�.+��� $��13(�$��Medium"�$��	FX Volume���� $��12(�#���  $"��Adjust ambient sound volume.���� $��1��$��0,8,16,0q�$	��200��$ 10Q�$ 0Q�$ 1Q� �� *ViewModel.FxVolume+.���� $����Ambients sounds that are not dialog (pve), music or voice chat. Sounds like these usually include footsteps, jumping, tires, reloading, etc...��$
 Left=�$
��Left=�$
��Wrap=�  Wpf.Ui.Controls.Snackbar  -     Name$ SnackBarFirstTime��$��2��$��56, 0q�   Title$ It's my first time!��$	��600��   
Appearance$ 	Secondary=�   CloseButtonEnabled$
 True��   Icon$ PuzzlePiece24=�   Message$U OBefore you dive into Just Cause, we need to set up some quick settings for you.��   Timeout$
 5000��