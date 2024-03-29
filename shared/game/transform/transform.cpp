#include <defs/standard.h>

#include "transform.h"

Transform::Transform(const vec3& position)
{
	m = mat4(1.f);

	m[3] = vec4(position, 1.f);
}

Transform::Transform(const vec3& position, const quat& rotation)
{
	m = glm::translate(m, position) * glm::mat4_cast(rotation);
}

Transform::Transform(const mat4& matrix) : m(matrix)
{
}

void Transform::decompose(vec3& t, quat& r, vec3& s) const
{
	t = m[3];
	s = { glm::length(vec3(m[0])), glm::length(vec3(m[1])), glm::length(vec3(m[2])) };
	r = glm::quat_cast(mat3(vec3(m[0]) / s.x, vec3(m[1]) / s.y, vec3(m[2]) / s.z));
}

void Transform::decompose(vec3& t, quat& r) const
{
	vec3 dummy;

	decompose(t, r, dummy);
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

void Transform::rotate(const vec3& v)
{
	m = glm::rotate(m, v.x, vec3(1.f, 0.f, 0.f));
	m = glm::rotate(m, v.y, vec3(0.f, 1.f, 0.f));
	m = glm::rotate(m, v.z, vec3(0.f, 0.f, 1.f));
}

void Transform::scale(const vec3& v)
{
	m = glm::scale(m, v);
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

quat Transform::get_rotation() const
{
	return glm::quat_cast(m);
}

TransformTR Transform::get_tr() const
{
	TransformTR tr {};

	decompose(tr.t, tr.r);

	return tr;
}

raw_mat4 Transform::to_raw() const
{
	raw_mat4 out;

	memcpy(&out, this, sizeof(out));

	return out;
}

#ifdef JC_CLIENT
Transform Transform::look_at(const vec3& eye, const vec3& target)
{
	Transform t;

	jc::this_call(jc::g::math::fn::LOOK_AT, &t, &eye, &target, &jc::vec::UP);

	return t;
}

vec3 Transform::rotate_point(vec3 p) const
{
	vec3 out;

	return *jc::c_call<vec3*, vec3*, vec3*, const Transform*>(jc::g::math::fn::ROTATE_POINT_BY_MATRIX, &out, &p, this);
}
#endif