#include <defs/standard.h>

#include <game/transform/transform.h>

#include "base.h"
#include "../character/character.h"
#include "../spawn_point/agent_spawn_point.h"
#include "../spawn_point/vehicle_spawn_point.h"

void ObjectBase::init_from_map(object_base_map* map)
{
	jc::v_call(this, jc::object_base::vt::INIT_FROM_MAP, map);
}

void ObjectBase::set_transform(const Transform& transform)
{
	switch (util::hash::JENKINS(get_typename()))
	{
	case Character::CLASS_ID():
	{
		jc::this_call<ptr>(jc::character::fn::SET_TRANSFORM, this, &transform);
		break;
	}
	case AgentSpawnPoint::CLASS_ID():
	case VehicleSpawnPoint::CLASS_ID():
	{
		jc::write(transform, this, jc::spawn_point::TRANSFORM);
		break;
	}
	default:
		log(RED, "'ObjectBase::{}' not implemented for type '{}'", CURR_FN, get_typename());
	}
}

void ObjectBase::set_position(const vec3& v)
{
	set_transform(Transform(v));
}

void ObjectBase::set_model(const std::string& name)
{
	switch (util::hash::JENKINS(get_typename()))
	{
	case Character::CLASS_ID():
	{
		BITCAST(Character*, this)->set_model(name);

		break;
	}
	default:
		log(RED, "'ObjectBase::{}' not implemented for type '{}'", CURR_FN, get_typename());
	}
}

bool ObjectBase::is_vehicle() const
{
	return jc::v_call<bool>(this, jc::object_base::vt::IS_VEHICLE);
}

const char* ObjectBase::get_typename() const
{
	const char* dummy = nullptr;

	return *jc::v_call<const char**>(this, jc::object_base::vt::GET_TYPENAME, &dummy);
}

Model* ObjectBase::get_model() const
{
	return jc::v_call<Model*>(this, jc::object_base::vt::GET_MODEL);
}

bref<Physical> ObjectBase::get_physical() const
{
	bref<Physical> tmp;

	jc::v_call(this, jc::object_base::vt::GET_PHYSICAL, &tmp);

	return tmp;
}

Transform ObjectBase::get_transform() const
{
	Transform transform;

	jc::v_call<bool>(this, jc::object_base::vt::GET_TRANSFORM, &transform);

	return transform;
}

vec3 ObjectBase::get_position() const
{
	return *get_transform().position();
}

vec3 ObjectBase::get_view_direction() const
{
	return *get_transform().forward();
}