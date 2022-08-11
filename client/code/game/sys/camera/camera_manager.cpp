#include <defs/standard.h>

#include "camera_manager.h"

void CameraManager::init()
{
}

void CameraManager::destroy()
{
}

void CameraManager::shake_main_camera(const vec3& position, float max_angle, float smoothness, float duration)
{
	jc::this_call<int>(jc::camera_manager::fn::CAMERA_SHAKE, jc::read<void*>(jc::camera_manager::CAM_SHAKE), &position, max_angle, smoothness, duration);
}

Camera* CameraManager::get_main_camera() const
{
	return jc::read<Camera*>(this, jc::camera_manager::MAIN_CAMERA);
}