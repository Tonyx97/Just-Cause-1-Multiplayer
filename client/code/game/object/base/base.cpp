#include <defs/standard.h>

#include <game/transform/transform.h>

#include "base.h"
#include "../character/character.h"
#include "../spawn_point/agent_spawn_point.h"
#include "../spawn_point/vehicle_spawn_point.h"
#include "../rigid_object/animated_rigid_object.h"
#include "../sound/sound_game_obj.h"
#include "../damageable_object/damageable_object.h"

bool object_base_map::find_string(uint32_t hash, jc::stl::string& out)
{
	return jc::this_call<bool>(FIND_STRING(), this, &hash, &out);
}

void object_base_map::walk()
{
	struct Node
	{
		Node* left,
			* parent,
			* right;
		ptr	  hash;
		int	  type;
		void* data;
		bool  unk2;
		bool  is_leaf;
	};

	Node* first = jc::read<Node*>(this, 0x4);
	Node* root = jc::read<Node*>(first, 0x4);

	std::vector<vec2> vec2s;
	std::vector<vec3> vec3s;
	std::vector<vec4> vec4s;
	std::vector<mat4> mat4s;

	std::map<ValueType, std::set<std::string>> inserts;

	auto convert_hash_to_name = [&](ptr hash)
	{
		static std::unordered_map<ptr, std::string> hash_to_name_map =
		{
			// ObjectBase

			{ ObjectBase::Hash_Class, "ObjectBase::Hash_Class" },
			{ ObjectBase::Hash_Model, "ObjectBase::Hash_Model" },
			{ ObjectBase::Hash_PD_Model, "ObjectBase::Hash_PD_Model" },
			{ ObjectBase::Hash_Desc, "ObjectBase::Hash_Desc" },
			{ ObjectBase::Hash_LOD_Model, "ObjectBase::Hash_LOD_Model" },
			{ ObjectBase::Hash_PFX, "ObjectBase::Hash_PFX" },
			{ ObjectBase::Hash_Transform, "ObjectBase::Hash_Transform" },
			{ ObjectBase::Hash_SpawnDistance, "ObjectBase::Hash_SpawnDistance" },
			{ ObjectBase::Hash_Relative, "ObjectBase::Hash_Relative" },
			{ ObjectBase::Hash_KeyObject, "ObjectBase::Hash_KeyObject" },
			{ ObjectBase::Hash_Enabled, "ObjectBase::Hash_Enabled" },

			// SpawnPoint

			{ SpawnPoint::Hash_MaxHealth, "SpawnPoint::Hash_MaxHealth" },

			// SoundGameObject

			{ SoundGameObject::Hash_SAB, "SoundGameObject::Hash_SAB" },
			{ SoundGameObject::Hash_SOB, "SoundGameObject::Hash_SOB" },
			{ SoundGameObject::Hash_SoundID, "SoundGameObject::Hash_SoundID" },
			{ SoundGameObject::Hash_MaxDistance, "SoundGameObject::Hash_MaxDistance" },
			{ SoundGameObject::Hash_SoundID, "SoundGameObject::Hash_SoundID" },
			{ SoundGameObject::Hash_Event0, "SoundGameObject::Hash_Event0" },
			{ SoundGameObject::Hash_Event1, "SoundGameObject::Hash_Event1" },
			{ SoundGameObject::Hash_Event2, "SoundGameObject::Hash_Event2" },
			{ SoundGameObject::Hash_Event3, "SoundGameObject::Hash_Event3" },
			{ SoundGameObject::Hash_Event4, "SoundGameObject::Hash_Event4" },
			{ SoundGameObject::Hash_Event5, "SoundGameObject::Hash_Event5" },
			{ SoundGameObject::Hash_Event6, "SoundGameObject::Hash_Event6" },
		};

		auto it = hash_to_name_map.find(hash);
		if (it == hash_to_name_map.end())
			return util::string::hex_to_str(hash);

		return it->second;
	};

	std::function<void(Node*)> walk_tree = [&](Node* n)
	{
		if (n->hash != first->hash)
		{
			auto type = jc::read<int>(n->data);

			std::string hash_or_name;

			switch (type)
			{
			case Int:		inserts[Int].insert(FORMATV("map.insert<object_base_map::Int>({}, {}); // int", convert_hash_to_name(n->hash), *(int*)(ptr(n->data) + 4))); break;
			case Float:		inserts[Float].insert(FORMATV("map.insert<object_base_map::Float>({}, {:.2f}f); // float", convert_hash_to_name(n->hash), *(float*)(ptr(n->data) + 4))); break;
			case String:	inserts[String].insert(FORMATV("map.insert<object_base_map::String>({}, R\"({})\"); // string", convert_hash_to_name(n->hash), (const char*)(*(ptr*)(ptr(n->data) + 4)))); break;
			case Vec2:
			{
				vec2s.push_back(**(vec2**)(ptr(n->data) + 4));
				inserts[Vec2].insert(FORMATV("map.insert<object_base_map::Vec2>({}, &v2_{}); // vec2", convert_hash_to_name(n->hash), vec2s.size() - 1u));
				break;
			}
			case Vec3:
			{
				vec3s.push_back(**(vec3**)(ptr(n->data) + 4));
				inserts[Vec3].insert(FORMATV("map.insert<object_base_map::Vec3>({}, &v3_{}); // vec3", convert_hash_to_name(n->hash), vec3s.size() - 1u));
				break;
			}
			case Vec4:
			{
				vec4s.push_back(**(vec4**)(ptr(n->data) + 4));
				inserts[Vec4].insert(FORMATV("map.insert<object_base_map::Vec4>({}, &v4_{}); // vec4", convert_hash_to_name(n->hash), vec4s.size() - 1u));
				break;
			}
			case Mat4:
			{
				mat4s.push_back(**(mat4**)(ptr(n->data) + 4));
				inserts[Mat4].insert(FORMATV("map.insert<object_base_map::Mat4>({}, &m{}); // mat4", convert_hash_to_name(n->hash), mat4s.size() - 1u));
				break;
			}
			default:
				log(RED, "// Unknown map entry with type {}", type);
			}
		}

		if (!n->is_leaf)
		{
			walk_tree(n->left);
			walk_tree(n->right);
		}
	};

	walk_tree(root);

	log(GREEN, "object_base_map map {{}};");

	for (int i = 0; const auto & v : mat4s)
		log(GREEN, "const auto m{} = mat4 {{ {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f }};", i++, v[0][0], v[0][1], v[0][2], v[0][3]
			, v[1][0], v[1][1], v[1][2], v[1][3]
			, v[2][0], v[2][1], v[2][2], v[2][3]
			, v[3][0], v[3][1], v[3][2], v[3][3]);

	for (int i = 0; const auto & v : vec4s)
		log(GREEN, "const auto v4_{} = vec4 {{ {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f }};", i++, v.x, v.y, v.z, v.w);

	for (int i = 0; const auto & v : vec3s)
		log(GREEN, "const auto v3_{} = vec3 {{ {:.2f}f, {:.2f}f, {:.2f}f }};", i++, v.x, v.y, v.z);

	for (int i = 0; const auto & v : vec2s)
		log(GREEN, "const auto v2_{} = vec2 {{ {:.2f}f, {:.2f}f }};", i++, v.x, v.y);

	for (const auto& [type, v] : inserts)
		for (const auto& str : v)
			log(GREEN, "{}", str);
}

void ObjectBase::init_from_map(object_base_map* map)
{
	jc::v_call(this, jc::object_base::vt::INIT_FROM_MAP, map);
}

void ObjectBase::set_transform(const Transform& transform)
{
	switch (get_typename_hash())
	{
	case Character::CLASS_ID():
	{
		jc::this_call<void>(jc::character::fn::SET_TRANSFORM, this, &transform);
		break;
	}
	default: jc::v_call(this, jc::object_base::vt::SET_TRANSFORM, &transform);
	}
}

void ObjectBase::set_position(const vec3& v)
{
	set_transform(Transform(v));
}

void ObjectBase::set_model(uint32_t id)
{
	switch (util::hash::JENKINS(get_typename()))
	{
	case Character::CLASS_ID():
	{
		BITCAST(Character*, this)->set_skin(id);

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

uint32_t ObjectBase::get_typename_hash() const
{
	const auto type_name = get_typename();
	return type_name ? util::hash::JENKINS(type_name) : 0u;
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
	return get_transform().get_position();
}

vec3 ObjectBase::get_view_direction() const
{
	return get_transform().forward();
}

quat ObjectBase::get_rotation() const
{
	return glm::quat_cast(get_transform().get_matrix());
}