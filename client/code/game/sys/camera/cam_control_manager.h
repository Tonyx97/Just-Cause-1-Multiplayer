#pragma once

namespace jc::cam_control_manager
{
	static constexpr uint32_t SINGLETON = 0xD8509C; // CamControlManager*

	namespace fn
	{
		static constexpr uint32_t CAMERA_SHAKE			= 0x583950;
		static constexpr uint32_t GET_ENTRY				= 0x582920;
	}

	namespace g
	{
		static constexpr uint32_t DEFAULT_FOCUS_HEIGHT			= 0xAED100;
		static constexpr uint32_t AIM_FOCUS_HEIGHT				= 0xAED144;
	}
}

DEFINE_ENUM(CameraEntryType, int32_t)
{
	CameraEntryType_Settings = 1,
};

class CamSettings;

class CamControlManager
{
public:

	void init();
	void destroy();
	void shake_cam(const vec3& position, float scale, float speed, float delta);
	void set_aim_focus_height(float v);
	void set_default_focus_height(float v);

	CamSettings* get_settings();
};

inline Singleton<CamControlManager, jc::cam_control_manager::SINGLETON> g_cam_control;