
namespace launcher.Models
{
    public class GameSettings
    {
        public string? GamePath { get; set; }
        public string? Username { get; set; }
        public bool MotionBlur { get; set; }
        public bool HeatHaze { get; set; }
        public bool PostFx { get; set; }
        public GameSettingTextureResolution TextureResolution { get; set; }
        public GameSettingSceneComplexity SceneComplexity { get; set; }
        public GameSettingWaterQuality WaterQuality { get; set; }
        public int FxVolume { get; set; }
    }
}
