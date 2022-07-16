#pragma once

#ifndef MAP_BASE_DEFINED
static_assert("map_base.h must be defined");
#endif

#define IMPL_OBJECT_TYPE_ID(x) static constexpr auto CLASS_NAME() { return x; } \
							   static constexpr auto CLASS_ID() { return util::hash::JENKINS(CLASS_NAME()); }

#include "obj_event_manager.h"

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
	enum ValueType
	{
		Int = 1,
		Float = 2,
		String = 3,
		Vec3 = 5,
		Mat4 = 8,
		Unknown = 0
	};

	static constexpr auto CREATE() { return 0x5CE160; }
	static constexpr auto DESTROY() { return 0x5CE110; }
	static constexpr auto INSERT() { return 0x5CE300; }
	static constexpr auto FIND_STRING() { return 0x46ADD0; }

	template <ValueType TYPE, typename T>
	void insert(uint32_t key, const T& value)
	{
		if constexpr (std::is_integral_v<T>)
			this->insert_impl(key, std::make_pair(TYPE, static_cast<int32_t>(value)));
		else if constexpr (std::is_same_v<T, std::string>)
			this->insert_impl(key, std::make_pair(TYPE, value.c_str()));
		else
			this->insert_impl(key, std::make_pair(TYPE, value));
	}

	bool find_string(uint32_t hash, jc::stl::string& out);

	void walk();
};

class Transform;
class Model;
class Physical;

class ObjectBase : public ObjectEventManager
{
public:

	enum Hash : uint32_t
	{
		Hash_Class = 0xef911d14,
		Hash_Model = 0xdfe26313,
		Hash_PD_Model = 0x206d0589,
		Hash_Desc = 0xb8fbd88e,
		Hash_LOD_Model = 0xea402acf,
		Hash_PFX = 0x5b982501,
		Hash_Transform = 0xacaefb1,
		Hash_SpawnDistance = 0xd2f9579a,
		Hash_Relative = 0x773aff1c,
		Hash_KeyObject = 0x69a3a614,
		Hash_Enabled = 0xee2cc81d,
	};

	void set_transform(const Transform& transform);
	void set_position(const vec3& v);
	void set_model(uint32_t id);
	void init_from_map(object_base_map* map);

	bool is_vehicle() const;

	uint32_t get_typename_hash() const;

	const char* get_typename() const;

	Model* get_model() const;

	bref<Physical> get_physical() const;

	Transform get_transform() const;

	vec3 get_position() const;
	vec3 get_view_direction() const;
};