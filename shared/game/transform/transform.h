#pragma once

struct TransformTR
{
	vec3 t = jc::vec::ZERO;
	quat r = jc::qua::IDENTITY;
};

class Transform
{
private:

	mat4 m = mat4(1.f);

public:
	Transform() {}
	Transform(const vec3& position);
	Transform(const vec3& position, const quat& rotation);
	Transform(const mat4& matrix);

	void compose(const vec3& t, const quat& r, const vec3& s);
	void decompose(vec3& t, quat& r, vec3& s) const;
	void decompose(vec3& t, quat& r) const;
	void translate(const vec3& v);

	/**
	* compares the current matrix with another one using thresholds for position and rotation
	* @return True if the matrices are different, otherwise it returns false
	*/
	bool compare_position_and_rotation(const Transform& right, float position_threshold, float rotation_threshold);

	Transform& interpolate(const Transform& transform, float tf = 1.f, float rf = 1.f, float sf = 1.f);

	vec3 right() const { return m[0]; }
	vec3 up() const { return m[1]; }
	vec3 forward() const { return m[2]; }
	vec3 get_position() const { return m[3]; }

	bool operator != (const Transform& v) const { return m != v.m; }

	quat get_rotation() const;

	TransformTR get_tr() const;

	const mat4& get_matrix() const { return m; }

	const vec4& row(int i) const { return m[i]; }

	const vec4& operator [](int i) const { return row(i); }

#ifdef JC_CLIENT
	vec3 rotate_point(vec3 p) const;
#endif
};