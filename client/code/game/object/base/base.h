#pragma once

#ifndef MAP_BASE_DEFINED
static_assert("map_base.h must be defined");
#endif

#define IMPL_OBJECT_TYPE_ID(x) static constexpr auto CLASS_NAME() { return x; } \
							   static constexpr auto CLASS_ID() { return util::hash::JENKINS(CLASS_NAME()); }

namespace jc::object_base
{
	static constexpr uint32_t EVENT_MANAGER = 0xC;

	namespace event_manager
	{
		namespace fn
		{
			static constexpr uint32_t CALL_EVENT = 0x407FD0;
		}

		namespace vt
		{
			static constexpr uint32_t DESTROY	= 0;
			static constexpr uint32_t CALL		= 1;
		}
	}

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
	HASH_CLASS				= 0xef911d14,
	HASH_MODEL				= 0xdfe26313,
	HASH_PD_MODEL			= 0x206d0589,
	HASH_DESCRIPTION		= 0xb8fbd88e,
	HASH_TRANSFORM			= 0xacaefb1,
	HASH_SPAWN_DISTANCE		= 0xd2f9579a,
	HASH_RELATIVE			= 0x773aff1c,
	HASH_MAX_HEALTH			= 0xe1f06be0,
	HASH_KEY_OBJECT			= 0x69a3a614,
};

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
				case Int:		inserts.push_back(FORMATV("map.insert<object_base_map::Int>(0x{:x}, {}); // int", n->hash, *(int*)(ptr(n->data) + 4))); break;
				case Float:		inserts.push_back(FORMATV("map.insert<object_base_map::Float>(0x{:x}, {:.2f}f); // float", n->hash, *(float*)(ptr(n->data) + 4))); break;
				case String:	inserts.push_back(FORMATV("map.insert<object_base_map::String>(0x{:x}, R\"({})\"); // string", n->hash, (const char*)(*(ptr*)(ptr(n->data) + 4)))); break;
				case Vec3:
				{
					vec3s.push_back(**(vec3**)(ptr(n->data) + 4));
					inserts.push_back(FORMATV("map.insert<object_base_map::Vec3>(0x{:x}, &v{}); // vec3", n->hash, vec3s.size() - 1u));
					break;
				}
				case Mat4:
				{
					mat4s.push_back(**(mat4**)(ptr(n->data) + 4));
					inserts.push_back(FORMATV("map.insert<object_base_map::Mat4>(0x{:x}, &m{}); // mat4", n->hash, mat4s.size() - 1u));
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

struct Event
{

};

struct ObjectEventManager
{
	void call_event(ptr offset, void* userdata = nullptr);
	void call_event_ex(Event* event_instance, void* userdata = nullptr);
	void call_event_ex(ptr offset, void* userdata = nullptr);

	Event* get_event(ptr offset);
};

class ObjectBase
{
public:
	void set_transform(const Transform& transform);
	void set_position(const vec3& v);
	void set_model(uint32_t id);
	void init_from_map(object_base_map* map);

	bool is_vehicle() const;

	const char* get_typename() const;

	ObjectEventManager* get_event_manager() const;

	Model* get_model() const;

	bref<Physical> get_physical() const;

	Transform get_transform() const;

	vec3 get_position() const;
	vec3 get_view_direction() const;
};