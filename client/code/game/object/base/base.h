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

		std::function<void(Node*)> walk_tree = [&](Node* n)
		{
			if (n->hash != first->hash)
			{
				auto type = jc::read<int>(n->data);

				switch (type)
				{
				case ValueType_Bool:	log(GREEN, "{} 0x{:x} -> bool/int {}", type, n->hash, *(bool*)(ptr(n->data) + 4)); break;
				case ValueType_Float:	log(GREEN, "{} 0x{:x} -> float/int16 {}", type, n->hash, *(float*)(ptr(n->data) + 4)); break;
				case ValueType_String:	log(GREEN, "{} 0x{:x} -> string {}", type, n->hash, (const char*)(*(ptr*)(ptr(n->data) + 4))); break;
				case ValueType_Vec3:
				{
					vec3 v = **(vec3**)(ptr(n->data) + 4);

					log(GREEN, "{} 0x{:x} -> vec3 {{ {:.2f}f, {:.2f}f, {:.2f}f }}", type, n->hash, v.x, v.y, v.z);
					
					break;
				}
				case ValueType_Mat4:
				{
					mat4 v = **(mat4**)(ptr(n->data) + 4);

					log(GREEN, R"({} 0x{:x} -> mat4 {{ {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f, {:.2f}f }})", type, n->hash, v[0][0], v[0][1], v[0][2], v[0][3]
																						, v[1][0], v[1][1], v[1][2], v[1][3]
																						, v[2][0], v[2][1], v[2][2], v[2][3]
																						, v[3][0], v[3][1], v[3][2], v[3][3]);
					
					break;
				}
				default:
					log(RED, "Unknown map entry with type {}", type);
				}
			}

			if (!n->is_leaf)
			{
				walk_tree(n->left);
				walk_tree(n->right);
			}
		};

		walk_tree(root);
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