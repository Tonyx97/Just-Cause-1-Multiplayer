#include <defs/standard.h>

#include "camera.h"

#include <game/transform/transform.h>
#include <game/sys/renderer/renderer.h>

bool Camera::w2s(const vec3& p, vec2& out)
{
	const auto width  = get_width(),
			   height = get_height();

	const auto proj_matrix = get_proj_matrix(),
			   view_matrix = get_view_matrix();

	const auto matrix = proj_matrix * view_matrix;

	vec3 res;

	D3DXVec3TransformCoord((D3DXVECTOR3*)&res, (D3DXVECTOR3*)&p, (D3DXMATRIX*)&matrix);

	out.x = 0.5f * (res.x + 1.f) * width;
	out.y = 0.5f * (1.f - res.y) * height;

	return res.z <= 1.f;
}

int Camera::get_width() const
{
	return jc::read<int>(this, jc::camera::WIDTH);
}

int Camera::get_height() const
{
	return jc::read<int>(this, jc::camera::HEIGHT);
}

mat4 Camera::get_model_matrix() const
{
	return jc::read<mat4>(this, jc::camera::MODEL_MATRIX);
}

mat4 Camera::get_view_matrix() const
{
	return jc::read<mat4>(this, jc::camera::VIEW_MATRIX);
}

mat4 Camera::get_proj_matrix() const
{
	return jc::read<mat4>(this, jc::camera::PROJ_MATRIX);
}

vec3 Camera::get_model_right_vector() const
{
	return jc::read<Transform>(this, jc::camera::MODEL_MATRIX).right();
}

vec3 Camera::get_model_up_vector() const
{
	return jc::read<Transform>(this, jc::camera::MODEL_MATRIX).up();
}

vec3 Camera::get_model_forward_vector() const
{
	return jc::read<Transform>(this, jc::camera::MODEL_MATRIX).forward();
}

vec3 Camera::get_view_right_vector() const
{
	return jc::read<Transform>(this, jc::camera::VIEW_MATRIX).right();
}

vec3 Camera::get_view_up_vector() const
{
	return jc::read<Transform>(this, jc::camera::VIEW_MATRIX).up();
}

vec3 Camera::get_view_forward_vector() const
{
	return jc::read<Transform>(this, jc::camera::VIEW_MATRIX).forward();
}

vec3 Camera::get_proj_right_vector() const
{
	return jc::read<Transform>(this, jc::camera::VIEW_MATRIX).right();
}
vec3 Camera::get_proj_up_vector() const
{
	return jc::read<Transform>(this, jc::camera::VIEW_MATRIX).up();
}
vec3 Camera::get_proj_forward_vector() const
{
	return jc::read<Transform>(this, jc::camera::VIEW_MATRIX).forward();
}