#pragma once

namespace jc::settings_system
{
	static constexpr uint32_t SINGLETON = 0xD32EE4; // AiCore*

	static constexpr uint32_t FX_VOLUME = 0x1C4; // float
	static constexpr uint32_t MUSIC_VOLUME = 0x1C8; // float
	static constexpr uint32_t DIALOGUE_VOLUME = 0x1CC; // float
	static constexpr uint32_t MOTION_BLUR = 0x1D0; // int
	static constexpr uint32_t ACTION_CAMERA = 0x1D4; // int
	static constexpr uint32_t SUBTITLES = 0x1D8; // int
	static constexpr uint32_t INVERT_UP_DOWN = 0x1DC; // int
	static constexpr uint32_t SHOW_HP_BAR = 0x1E4; // int
	static constexpr uint32_t SHOW_WEAPON_SELECTOR = 0x1E8; // int
	static constexpr uint32_t SHOW_OBJECTIVE_INFO = 0x1EC; // int
	static constexpr uint32_t SHOW_MINIMAP = 0x1F0; // int
	static constexpr uint32_t SHOW_ACTION_ICONS = 0x1F4; // int
	static constexpr uint32_t SCREEN_RESOLUTION = 0x200; // int
	static constexpr uint32_t HEAT_HAZE = 0x204; // int
	static constexpr uint32_t WATER_QUALITY = 0x208; // int
	static constexpr uint32_t TERRAIN_DETAIL = 0x20C; // int
	static constexpr uint32_t SCENE_COMPLEXITY = 0x210; // int
	static constexpr uint32_t TEXTURE_RESOLUTION = 0x214; // int
	static constexpr uint32_t POST_FX_ENABLED = 0x218; // int
}

enum SettingType
{
	SettingType_FxVolume,
	SettingType_MusicVolume,
	SettingType_DialogueVolume,
	SettingType_MotionBlur,
	SettingType_ActionCamera,
	SettingType_Subtitles,
	SettingType_InvertUpDown,
	SettingType_ShowHpBar,
	SettingType_ShowWeaponSelector,
	SettingType_ShowObjectiveInfo,
	SettingType_ShowMinimap,
	SettingType_ShowActionIcons,
	SettingType_ScreenResolution,
	SettingType_HeatHaze,
	SettingType_WaterQuality,
	SettingType_TerrainQuality,
	SettingType_SceneComplexity,
	SettingType_TextureResolution,
	SettingType_PostFX,
};

class SettingsSystem
{
public:

	void init();
	void destroy();

	void set_float(int32_t type, float v);
	void set_int(int32_t type, int32_t v);
};

inline Singleton<SettingsSystem, jc::settings_system::SINGLETON> g_settings;