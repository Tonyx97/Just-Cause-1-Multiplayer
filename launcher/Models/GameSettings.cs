
namespace launcher.Models
{
    public class GameSettings
    {
        // Empty (default) constructor

        public GameSettings()
        {
            GamePath = string.Empty;
            Username = string.Empty;
            MotionBlur = true;
            HeatHaze = true;
            PostFx = true;
            TextureResolution = GameSettingTextureResolution.High;
            SceneComplexity = GameSettingSceneComplexity.High;
            WaterQuality = GameSettingWaterQuality.High;
            Volume = 10; // 0 - 10
            FxVolume = 10; // 0 - 10
        }

        public string GamePath { get; set; }
        public string Username { get; set; }
        public bool MotionBlur { get; set; }
        public bool HeatHaze { get; set; }
        public bool PostFx { get; set; }
        public GameSettingTextureResolution TextureResolution { get; set; }
        public GameSettingSceneComplexity SceneComplexity { get; set; }
        public GameSettingWaterQuality WaterQuality { get; set; }
        public int Volume { get; set; }
        public int FxVolume { get; set; }
    }
}
