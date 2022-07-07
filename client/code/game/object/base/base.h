#pragma once

#ifndef MAP_BASE_DEFINED
static_assert("map_base.h must be defined");
#endif

#define IMPL_OBJECT_TYPE_ID(x) static constexpr auto CLASS_NAME() { return x; } \
							   static constexpr auto CLASS_ID() { return util::hash::JENKINS(CLASS_NAME()); }

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

enum Hash : uint32_t
{
	HASH_CLASS				= 0xEF911D14,
	HASH_MODEL				= 0xDFE26313,
	HASH_PD_MODEL			= 0x206D0589,
};

struct object_base_map : public jc::map<object_base_map, uint32_t>
{
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

	bool find_string(uint32_t hash, jc::stl::string& out)
	{
		return jc::this_call<bool>(FIND_STRING(), this, &hash, &out);
	}

	void walk()
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

		log(GREEN, "object_base_map map {{}};");

		std::vector<vec3> vec3s;
		std::vector<mat4> mat4s;

		std::vector<std::string> inserts;

		std::function<void(Node*)> walk_tree = [&](Node* n)
		{
			if (n->hash != first->hash)
			{
				auto type = jc::read<int>(n->data);
				
				switch (type)
				{
				case ValueType_Int:		inserts.push_back(FORMATV("map.insert<ValueType_Int>(0x{:x}, {}); // int", n->hash, *(int*)(ptr(n->data) + 4))); break;
				case ValueType_Float:	inserts.push_back(FORMATV("map.insert<ValueType_Float>(0x{:x}, {:.2f}f); // float", n->hash, *(float*)(ptr(n->data) + 4))); break;
				case ValueType_String:	inserts.push_back(FORMATV("map.insert<ValueType_String>(0x{:x}, R\"({})\"); // string", n->hash, (const char*)(*(ptr*)(ptr(n->data) + 4)))); break;
				case ValueType_Vec3:
				{
					vec3s.push_back(**(vec3**)(ptr(n->data) + 4));

					inserts.push_back(FORMATV("map.insert<ValueType_Vec3>(0x{:x}, &v{}); // vec3", n->hash, vec3s.size() - 1u));
					
					break;
				}
				case ValueType_Mat4:
				{
					mat4s.push_back(**(mat4**)(ptr(n->data) + 4));
					inserts.push_back(FORMATV("map.insert<ValueType_Mat4>(0x{:x}, &m{}); // mat4", n->hash, mat4s.size() - 1u));

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

		for (int i = 0; const auto & v : mat4s)
			log(GREEN, "const auto m{} = mat4 {{ {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f }};", i++, v[0][0], v[0][1], v[0][2], v[0][3]
				, v[1][0], v[1][1], v[1][2], v[1][3]
				, v[2][0], v[2][1], v[2][2], v[2][3]
				, v[3][0], v[3][1], v[3][2], v[3][3]);

		for (int i = 0; const auto & v : vec3s)
			log(GREEN, "const auto v{} = vec3 {{ {:.2f}f, {:.2f}f, {:.2f}f }};", i++, v.x, v.y, v.z);

		for (const auto& v : inserts)
			log(GREEN, "{}", v);
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
	void set_model(uint32_t id);
	void init_from_map(object_base_map* map);

	bool is_vehicle() const;

	const char* get_typename() const;

	Model* get_model() const;

	bref<Physical> get_physical() const;

	Transform get_transform() const;

	vec3 get_position() const;
	vec3 get_view_direction() const;
};