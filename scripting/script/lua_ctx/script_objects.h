#pragma once

template <typename T>
struct script_object
{
	using base_object_type = T;

	base_object_type c_obj {};

	script_object() {}

	virtual ~script_object() = default;

	virtual const base_object_type& obj() const = 0;
};

struct svec3 : public script_object<vec3>
{
	svec3() {}
	svec3(const base_object_type& v) { c_obj = v; }
	svec3(float x, float y, float z) { c_obj = { x, y, z }; }

	const base_object_type& obj() const override { return c_obj; }

	inline float length() const { return glm::length(c_obj); }
	inline float distance(const svec3& v) const { return glm::distance(c_obj, v.c_obj); }
	inline float distance_sqr(const svec3& v) const { return glm::distance2(c_obj, v.c_obj); }

	inline void set_x(float v) { c_obj.x = v; }
	inline void set_y(float v) { c_obj.y = v; }
	inline void set_z(float v) { c_obj.z = v; }

	inline float get_x() const { return c_obj.x; }
	inline float get_y() const { return c_obj.y; }
	inline float get_z() const { return c_obj.z; }
};