using System;
using System.Drawing.Printing;
using System.Threading.Tasks;
using launcher.Models;
using Microsoft.Win32;

namespace launcher.Services.Repositories
{
    public class RegistryRepositoryService : IRepositoryService
    {
        private readonly static string _softwareKey = "SOFTWARE";
        private readonly static string _directoryKey = "JCMP1";
        private readonly static string _gameDirectoryKey = "Game";

        // IRepositoryService
        //
        public Task<bool> IsReachableAsync()
        {
            return Task.Factory.StartNew(() =>
            {
                try
                {
                    // Create a RegistryKey, which will access the HKEY_CURRENT_USER
                    // key in the registry of this machine. If we can open/access it
                    // we have enough permission and repository is reachable.
                    //
                    var currentUser = Registry.CurrentUser;
                    if (currentUser == null)
                    {
                        return false;
                    }
                    var softwareDirectory = currentUser.OpenSubKey(_softwareKey);
                    if (softwareDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        currentUser.Close();
                        return false;
                    }
                    softwareDirectory.Close();
                    currentUser.Close();
                    return true;
                }
                catch (Exception)
                {
                    return false;
                }
            });
        }

        public async Task<bool> IsInitializedAsync()
        {
            // Make sure repository is reachable.
            //
            if(await IsReachableAsync() == false)
            {
                return false;
            }

            return await Task.Factory.StartNew(() =>
            {
                try
                {
                    // Create a RegistryKey, which will access the HKEY_CURRENT_USER
                    // key in the registry of this machine. If we can open/access it
                    // we have enough permission and repository is reachable.
                    //
                    var currentUser = Registry.CurrentUser;
                    if(currentUser == null)
                    {
                        // Not initialized, or not enough permissions.
                        //
                        return false;
                    }
                    var softwareDirectory = currentUser.OpenSubKey(_softwareKey);
                    if (softwareDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        currentUser.Close();
                        return false;
                    }
                    var jcmpDirectory = softwareDirectory.OpenSubKey(_directoryKey);
                    if(jcmpDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        softwareDirectory.Close();
                        currentUser.Close();
                        return false;
                    }
                    var gameDirectory = jcmpDirectory.OpenSubKey(_gameDirectoryKey);
                    if (gameDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        currentUser.Close();
                        softwareDirectory.Close();
                        jcmpDirectory.Close();
                        gameDirectory?.Close();
                        return false;
                    }

                    // Initialized properly if username and game path are populated.
                    //
                    if(gameDirectory.GetValue(nameof(GameSettings.GamePath)) is string gamePathUnwrapped)
                    {
                        if (gamePathUnwrapped.Length == 0) return false;
                    }
                    if (gameDirectory.GetValue(nameof(GameSettings.Username)) is string usernameUnwrapped)
                    {
                        if (usernameUnwrapped.Length == 0) return false;
                    }

                    return true;
                }
                catch (Exception)
                {
                }

                return false;
            });
        }

        public async Task<bool> InitializeAsync()
        {
            // Make sure repository is reachable.
            //
            if (await IsInitializedAsync())
            {
                // If it is already initialized, do not perform additional work.
                //
                return true;
            }

            return await Task.Factory.StartNew(() =>
            {
                try
                {
                    // Create a RegistryKey, which will access the HKEY_CURRENT_USER
                    // key in the registry of this machine. If we can open/access it
                    // we have enough permission and repository is reachable.
                    //
                    var currentUser = Registry.CurrentUser;
                    if (currentUser == null)
                    {
                        // Not initialized, or not enough permissions.
                        //
                        return false;
                    }
                    var softwareDirectory = currentUser.OpenSubKey(_softwareKey, true);
                    if (softwareDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        currentUser.Close();
                        return false;
                    }
                    var jcmpDirectory = softwareDirectory.CreateSubKey(_directoryKey, true);
                    if (jcmpDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        softwareDirectory.Close();
                        currentUser.Close();
                        return false;
                    }
                    var gameDirectory = jcmpDirectory.CreateSubKey(_gameDirectoryKey, true);
                    if (gameDirectory == null)
                    {
                        // We could not initialized the repository for this session.
                        //
                        currentUser.Close();
                        softwareDirectory.Close();
                        jcmpDirectory.Close();
                        gameDirectory?.Close();
                        return false;
                    }
                    return true;
                }
                catch (Exception)
                {
                    return false;
                }
            });
        }

        public async Task<bool> InsertOrUpdateData(GameSettings gameSettings)
        {
            // Make sure repository is reachable.
            //
            if (await IsReachableAsync() == false)
            {
                // If it is already initialized, do not perform additional work.
                //
                return false;
            }

            return await Task.Factory.StartNew(() =>
            {
                try
                {
                    // Create a RegistryKey, which will access the HKEY_CURRENT_USER
                    // key in the registry of this machine. If we can open/access it
                    // we have enough permission and repository is reachable.
                    //
                    var currentUser = Registry.CurrentUser;
                    if (currentUser == null)
                    {
                        // Not initialized, or not enough permissions.
                        //
                        return false;
                    }
                    var softwareDirectory = currentUser.OpenSubKey(_softwareKey);
                    if (softwareDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        currentUser.Close();
                        return false;
                    }
                    var jcmpDirectory = softwareDirectory.OpenSubKey(_directoryKey);
                    if (jcmpDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        softwareDirectory.Close();
                        currentUser.Close();
                        return false;
                    }
                    var gameDirectory = jcmpDirectory.OpenSubKey(_gameDirectoryKey, true);
                    if (gameDirectory == null)
                    {
                        // We could not initialized the repository for this session.
                        //
                        currentUser.Close();
                        softwareDirectory.Close();
                        jcmpDirectory.Close();
                        gameDirectory?.Close();
                        return false;
                    }

                    // Write values.
                    //
                    gameDirectory.SetValue(nameof(gameSettings.GamePath), gameSettings.GamePath, RegistryValueKind.String);
                    gameDirectory.SetValue(nameof(gameSettings.Username), gameSettings.Username, RegistryValueKind.String);
                    gameDirectory.SetValue(nameof(gameSettings.MotionBlur), gameSettings.MotionBlur, RegistryValueKind.DWord);
                    gameDirectory.SetValue(nameof(gameSettings.HeatHaze), gameSettings.HeatHaze, RegistryValueKind.DWord);
                    gameDirectory.SetValue(nameof(gameSettings.PostFx), gameSettings.PostFx, RegistryValueKind.DWord);
                    gameDirectory.SetValue(nameof(gameSettings.TextureResolution), gameSettings.TextureResolution, RegistryValueKind.DWord);
                    gameDirectory.SetValue(nameof(gameSettings.SceneComplexity), gameSettings.SceneComplexity, RegistryValueKind.DWord);
                    gameDirectory.SetValue(nameof(gameSettings.WaterQuality), gameSettings.WaterQuality, RegistryValueKind.DWord);
                    gameDirectory.SetValue(nameof(gameSettings.FxVolume), gameSettings.FxVolume, RegistryValueKind.DWord);

                    return true;
                }
                catch (Exception)
                {
                    return false;
                }
            });
        }

        public async Task<GameSettings?> FetchDataAsync()
        {
            GameSettings? gameSettings = null;
            // Make sure repository is reachable.
            //
            if (await IsInitializedAsync() == false)
            {
                // If it is already initialized, do not perform additional work.
                //
                return gameSettings;
            }

            return await Task.Factory.StartNew(() =>
            {
                try
                {
                    // Create a RegistryKey, which will access the HKEY_CURRENT_USER
                    // key in the registry of this machine. If we can open/access it
                    // we have enough permission and repository is reachable.
                    //
                    var currentUser = Registry.CurrentUser;
                    if (currentUser == null)
                    {
                        // Not initialized, or not enough permissions.
                        //
                        return gameSettings;
                    }
                    var softwareDirectory = currentUser.OpenSubKey(_softwareKey);
                    if (softwareDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        currentUser.Close();
                        return gameSettings;
                    }
                    var jcmpDirectory = softwareDirectory.OpenSubKey(_directoryKey);
                    if (jcmpDirectory == null)
                    {
                        // We never initialized the repository for this session.
                        //
                        softwareDirectory.Close();
                        currentUser.Close();
                        return gameSettings;
                    }
                    var gameDirectory = jcmpDirectory.OpenSubKey(_gameDirectoryKey);
                    if (gameDirectory == null)
                    {
                        // We could not initialized the repository for this session.
                        //
                        currentUser.Close();
                        softwareDirectory.Close();
                        jcmpDirectory.Close();
                        gameDirectory?.Close();
                        return gameSettings;
                    }

                    // Read values.
                    //
                    gameSettings = new GameSettings();
                    gameSettings.GamePath = (string)gameDirectory.GetValue(nameof(gameSettings.GamePath));
                    gameSettings.Username = (string)gameDirectory.GetValue(nameof(gameSettings.Username));
                    gameSettings.MotionBlur = Convert.ToBoolean(gameDirectory.GetValue(nameof(gameSettings.MotionBlur)));
                    gameSettings.HeatHaze = Convert.ToBoolean(gameDirectory.GetValue(nameof(gameSettings.HeatHaze)));
                    gameSettings.PostFx = Convert.ToBoolean(gameDirectory.GetValue(nameof(gameSettings.PostFx)));
                    gameSettings.TextureResolution = (GameSettingTextureResolution)gameDirectory.GetValue(nameof(gameSettings.TextureResolution));
                    gameSettings.SceneComplexity = (GameSettingSceneComplexity)gameDirectory.GetValue(nameof(gameSettings.SceneComplexity));
                    gameSettings.WaterQuality = (GameSettingWaterQuality)gameDirectory.GetValue(nameof(gameSettings.WaterQuality));
                    gameSettings.FxVolume = (int)gameDirectory.GetValue(nameof(gameSettings.FxVolume));

                    return gameSettings;
                }
                catch (Exception)
                {
                    return null;
                }
            });
        }
    }
}
