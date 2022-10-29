#include <defs/standard.h>

#include "cam_control_manager.h"

void CamControlManager::init()
{
}

void CamControlManager::destroy()
{
}

void CamControlManager::shake_cam(const vec3& position, float scale, float speed, float delta)
{
	jc::this_call(jc::cam_control_manager::fn::CAMERA_SHAKE, this, &position, scale, speed, delta);
}

void CamControlManager::set_aim_focus_height(float v)
{
	jc::write(v, jc::cam_control_manager::g::AIM_FOCUS_HEIGHT);
}

void CamControlManager::set_default_focus_height(float v)
{
	jc::write(v, jc::cam_control_manager::g::DEFAULT_FOCUS_HEIGHT);
}

CamSettings* CamControlManager::get_settings()
{
	return jc::this_call<CamSettings*>(jc::cam_control_manager::fn::GET_ENTRY, this, CameraEntryType_Settings);
}