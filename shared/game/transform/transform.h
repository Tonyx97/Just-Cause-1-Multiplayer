#pragma once

#include <shared_mp/packets/base.h>

class Transform : public PacketBase
{
private:
	mat4 m;

public:
	Transform() {}
	Transform(const vec3& position);

	void compose(const vec3& t, const quat& r, const vec3& s);
	void decompose(vec3& t, quat& r, vec3& s) const;
	void interpolate(const Transform& transform, float tf = 1.f, float rf = 1.f, float sf = 1.f);
	void translate(const vec3& v);

	vec3 right();
	vec3 up();
	vec3 forward();
	vec3 position();

	const mat4& get_matrix() const { return m; }

#ifdef JC_CLIENT
	vec3 rotate_point(vec3 p) const;
#endif
};