#pragma once

namespace jc::camera
{
	static constexpr uint32_t MODEL_MATRIX = 0x14; // mat4
	static constexpr uint32_t PROJ_MATRIX  = 0x54; // mat4
	static constexpr uint32_t VIEW_MATRIX  = 0x94; // mat4
	static constexpr uint32_t WIDTH		   = 0x3A4; // float
	static constexpr uint32_t HEIGHT	   = WIDTH + 0x4; // float

	namespace vt
	{
	}
}

class Camera
{
public:

	bool w2s(const vec3& p, vec2& out);

	int get_width() const;
	int get_height() const;

	float get_yaw() const;

	mat4 get_model_matrix() const;
	mat4 get_proj_matrix() const;
	mat4 get_view_matrix() const;

	vec3 get_model_right_vector() const;
	vec3 get_model_up_vector() const;
	vec3 get_model_forward_vector() const;
	vec3 get_model_translation() const;

	vec3 get_proj_right_vector() const;
	vec3 get_proj_up_vector() const;
	vec3 get_proj_forward_vector() const;

	vec3 get_view_right_vector() const;
	vec3 get_view_up_vector() const;
	vec3 get_view_forward_vector() const;
};