#pragma once

class Transform
{
private:

	mat4 m = mat4(1.f);

public:
	Transform() {}
	Transform(const vec3& position);

	void compose(const vec3& t, const quat& r, const vec3& s);
	void decompose(vec3& t, quat& r, vec3& s) const;
	void translate(const vec3& v);

	/**
	* compares the current matrix with another one using thresholds for position and rotation
	* @return True if the matrices are different, otherwise it returns false
	*/
	bool compare_position_and_rotation(const Transform& right, float position_threshold, float rotation_threshold);

	Transform& interpolate(const Transform& transform, float tf = 1.f, float rf = 1.f, float sf = 1.f);

	vec3 right();
	vec3 up();
	vec3 forward();
	vec3 position();

	bool operator != (const Transform& v) const { return m != v.m; }

	const mat4& get_matrix() const { return m; }

#ifdef JC_CLIENT
	vec3 rotate_point(vec3 p) const;
#endif
};