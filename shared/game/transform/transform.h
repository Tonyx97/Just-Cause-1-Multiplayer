#pragma once

#include <shared_mp/packets/base.h>

class Transform : public PacketBase
{
private:
	mat4 m;

public:
	Transform() {}
	Transform(const vec3& position);

	void scale(const mat4& v);

	vec3* right();
	vec3* up();
	vec3* forward();
	vec3* position();

	const mat4* get_matrix() const { return &m; }

#ifdef JC_CLIENT
	vec3 rotate_point(vec3 p) const;
#endif
};