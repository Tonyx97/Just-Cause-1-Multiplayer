#pragma once

#ifndef MAP_BASE_DEFINED
static_assert("map_base.h must be defined");
#endif

namespace jc::object_base
{
	namespace vt
	{
		static constexpr uint32_t DESTROY		= 0;
		static constexpr uint32_t GET_MODEL		= 1;
		static constexpr uint32_t GET_PHYSICAL	= 3;
		static constexpr uint32_t GET_TRANSFORM = 4;
		static constexpr uint32_t INIT_FROM_MAP = 6;
		static constexpr uint32_t IS_VEHICLE	= 20;
		static constexpr uint32_t GET_TYPENAME	= 30;
	}
}

struct object_base_map : public jc::map<object_base_map, uint32_t>
{
	static constexpr auto CREATE() { return 0x5CE160; }
	static constexpr auto DESTROY() { return 0x5CE110; }
	static constexpr auto INSERT() { return 0x5CE300; }

	template <ValueType TYPE, typename T>
	void insert(uint32_t key, const T& value)
	{
		if constexpr (std::is_integral_v<T>)
			this->insert_impl(key, std::make_pair(TYPE, static_cast<int32_t>(value)));
		else
			this->insert_impl(key, std::make_pair(TYPE, value));
	}
};

class Transform;
class Model;
class Physical;

class ObjectBase
{
public:
	void set_transform(const Transform& transform);
	void set_position(const vec3& v);
	void set_model(const std::string& name);
	void init_from_map(object_base_map* map);

	bool is_vehicle() const;

	const char* get_typename() const;

	Model* get_model() const;

	bref<Physical> get_physical() const;

	Transform get_transform() const;

	vec3 get_position() const;
	vec3 get_view_direction() const;
};