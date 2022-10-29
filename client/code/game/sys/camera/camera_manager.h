#pragma once

namespace jc::camera_manager
{
	static constexpr uint32_t SINGLETON = 0xAF23DC; // CameraManager*

	static constexpr uint32_t ACTIVE_CAMERA = 0x8; // Camera*

	namespace fn
	{
	}
}

class Camera;

class CameraManager
{
public:

	void init();
	void destroy();

	Camera* get_active_camera() const;
};

inline Singleton<CameraManager, jc::camera_manager::SINGLETON> g_camera;