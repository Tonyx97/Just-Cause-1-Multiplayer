#pragma once

namespace jc::camera_manager
{
	static constexpr uint32_t SINGLETON = 0xAF23DC; // CameraManager*
	static constexpr uint32_t CAM_SHAKE = 0xD8509C; // CameraShake*

	static constexpr uint32_t MAIN_CAMERA = 0x8; // Camera*

	namespace fn
	{
		static constexpr uint32_t CAMERA_SHAKE = 0x583950;
	}
}

class Camera;

class CameraManager
{
public:
	void init();
	void destroy();
	void shake_main_camera(const vec3& position, float max_angle, float smoothness, float duration);

	Camera* get_main_camera() const;
};

inline Singleton<CameraManager, jc::camera_manager::SINGLETON> g_camera;