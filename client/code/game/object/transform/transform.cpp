#include <defs/standard.h>

#include <game/globals.h>

#include "transform.h"

Transform::Transform(const vec3& position)
{
	m = mat4(1.f);

	jc::write(position, &m._41);
}

vec3* Transform::right()
{
	return REFNO(vec3*, &m._11);
}

vec3* Transform::up()
{
	return REFNO(vec3*, &m._21);
}

void Transform::scale(const mat4& v)
{
	m *= v;
}

vec3* Transform::forward()
{
	return REFNO(vec3*, &m._31);
}

vec3* Transform::position()
{
	return REFNO(vec3*, &m._41);
}

vec3 Transform::rotate_point(vec3 p) const
{
	return *jc::c_call<vec3*, vec3*, vec3*, const Transform*>(jc::g::math::fn::ROTATE_POINT_BY_MATRIX, &p, &p, this);
}