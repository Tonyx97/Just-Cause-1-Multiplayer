#include <defs/standard.h>

#include "settings_system.h"

void SettingsSystem::init()
{
}

void SettingsSystem::destroy()
{
}

void SettingsSystem::set_float(int32_t type, float v)
{
	switch (type)
	{
	case SettingType_FxVolume:			jc::write(v, this, jc::settings_system::FX_VOLUME); return;
	case SettingType_MusicVolume:		jc::write(v, this, jc::settings_system::MUSIC_VOLUME); return;
	case SettingType_DialogueVolume:	jc::write(v, this, jc::settings_system::DIALOGUE_VOLUME); return;
	}
}

void SettingsSystem::set_int(int32_t type, int32_t v)
{
	switch (type)
	{
	case SettingType_MotionBlur:			jc::write(v, this, jc::settings_system::MOTION_BLUR); return;
	case SettingType_ActionCamera:			jc::write(v, this, jc::settings_system::ACTION_CAMERA); return;
	case SettingType_Subtitles:				jc::write(v, this, jc::settings_system::SUBTITLES); return;
	case SettingType_InvertUpDown:			jc::write(v, this, jc::settings_system::INVERT_UP_DOWN); return;
	case SettingType_ShowHpBar:				jc::write(v, this, jc::settings_system::SHOW_HP_BAR); return;
	case SettingType_ShowWeaponSelector:	jc::write(v, this, jc::settings_system::SHOW_WEAPON_SELECTOR); return;
	case SettingType_ShowObjectiveInfo:		jc::write(v, this, jc::settings_system::SHOW_OBJECTIVE_INFO); return;
	case SettingType_ShowMinimap:			jc::write(v, this, jc::settings_system::SHOW_MINIMAP); return;
	case SettingType_ShowActionIcons:		jc::write(v, this, jc::settings_system::SHOW_ACTION_ICONS); return;
	case SettingType_ScreenResolution:		jc::write(v, this, jc::settings_system::SCREEN_RESOLUTION); return;
	case SettingType_HeatHaze:				jc::write(v, this, jc::settings_system::HEAT_HAZE); return;
	case SettingType_WaterQuality:			jc::write(v, this, jc::settings_system::WATER_QUALITY); return;
	case SettingType_TerrainQuality:		jc::write(v, this, jc::settings_system::TERRAIN_DETAIL); return;
	case SettingType_SceneComplexity:		jc::write(v, this, jc::settings_system::SCENE_COMPLEXITY); return;
	case SettingType_TextureResolution:		jc::write(v, this, jc::settings_system::TEXTURE_RESOLUTION); return;
	case SettingType_PostFX:				jc::write(v, this, jc::settings_system::POST_FX_ENABLED); return;
	}
}