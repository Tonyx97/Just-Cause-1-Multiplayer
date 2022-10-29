#include <defs/standard.h>

#include "camera_manager.h"

void CameraManager::init()
{
}

void CameraManager::destroy()
{
}

Camera* CameraManager::get_active_camera() const
{
	return jc::read<Camera*>(this, jc::camera_manager::ACTIVE_CAMERA);
}