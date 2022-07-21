#include <defs/standard.h>

#include "transform.h"

Transform::Transform(const vec3& position)
{
	m = mat4(1.f);

	m[3] = vec4(position, 1.f);
}

void Transform::decompose(vec3& t, quat& r, vec3& s) const
{
	t = m[3];
	s = { glm::length(vec3(m[0])), glm::length(vec3(m[1])), glm::length(vec3(m[2])) };
	r = glm::quat_cast(mat3(vec3(m[0]) / s.x, vec3(m[1]) / s.y, vec3(m[2]) / s.z));
}

void Transform::compose(const vec3& t, const quat& r, const vec3& s)
{
	mat4 mat_t = glm::translate(mat4(1.f), t),
		 mat_r = glm::mat4_cast(r),
		 mat_s = glm::scale(mat4(1.f), s);

	m = mat_t * mat_r * mat_s;
}

void Transform::translate(const vec3& v)
{
	m = glm::translate(m, v);
}

bool Transform::compare_position_and_rotation(const Transform& right, float position_threshold, float rotation_threshold)
{
	vec3 t0, t1, s0, s1;
	quat q0, q1;

	decompose(t0, q0, s0);
	right.decompose(t1, q1, s1);

	return	glm::distance2(t0, t1) > position_threshold ||
			jc::math::quat_diff(q0, q1) > rotation_threshold;
}

Transform& Transform::interpolate(const Transform& transform, float tf, float rf, float sf)
{
	vec3 t0, t1, s0, s1;
	quat r0, r1;

	decompose(t0, r0, s0);
	transform.decompose(t1, r1, s1);

	r0 = glm::normalize(r0);
	r1 = glm::normalize(r1);

	const auto interpolated_t = glm::lerp(t0, t1, tf);
	const auto interpolated_r = glm::normalize(glm::slerp(r0, r1, rf));
	const auto interpolated_s = glm::lerp(s0, s1, sf);

	compose(interpolated_t, interpolated_r, interpolated_s);

	return *this;
}

vec3 Transform::right()
{
	return m[0];
}

vec3 Transform::up()
{
	return m[1];
}

vec3 Transform::forward()
{
	return m[2];
}

vec3 Transform::position()
{
	return m[3];
}

#ifdef JC_CLIENT
vec3 Transform::rotate_point(vec3 p) const
{
	return *jc::c_call<vec3*, vec3*, vec3*, const Transform*>(jc::g::math::fn::ROTATE_POINT_BY_MATRIX, &p, &p, this);
}
#endif