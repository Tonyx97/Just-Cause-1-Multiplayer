#include <defs/standard.h>

#include "clean_dbg.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/sys/all.h>

#include <game/object/base/base.h>
#include <game/object/item/item_pickup.h>

#define STRINGIFY(x) #x

#define EXPAND(x) STRINGIFY(x)

#define STRING_DUMP_PATH "\\reversing\\docs\\JC\\game\\string_dump.txt"
#define FILE_DUMP_PATH "\\reversing\\docs\\JC\\game\\file_dump.txt"
#define HASH_DUMP_PATH "\\reversing\\docs\\JC\\game\\hash_dump.txt"

std::map<std::string, void*> g_strings_dump;
std::set<std::string>		 g_files_dump;

std::mutex		 g_string_dump_lock;
std::mutex		 g_hash_dump_lock;
std::atomic_bool g_enable_string_dump = false;
std::atomic_bool g_enable_hash_dump = true;
std::atomic_bool g_record_object_map = false;

std::map<std::string, std::pair<uint32_t, ptr>> g_hashes;

constexpr bool ENABLE_HOOKS = false;
constexpr bool ENABLE_RAYCAST_DEBUG = false;
constexpr bool ENABLE_DUMPING = false;
constexpr bool ENABLE_INIT_FROM_MAP_DUMP = true;
constexpr bool ENABLE_STR_DEBUG = false;
constexpr bool ENABLE_MAP_DEBUG = false;

inline std::string get_solution_dir()
{
	auto solution_dir = std::string(EXPAND(SOLUTION_DIR));

	solution_dir.erase(0, 1);
	solution_dir.erase(solution_dir.size() - 2);

	return solution_dir;
}

DEFINE_HOOK_CCALL(print_error, 0x40D0B0, void, const char* text, ...)
{
	if (!text)
		return;

	switch (ptr(_ReturnAddress()))
	{
	case 0x45e67c:
	case 0x4d9469:
	case 0x4122ab:
	case 0x41228a:
	case 0x46e463:
	case 0x41183a:
	case 0x4117f4:
	case 0x4117ae:
	case 0x411768:
	case 0x411722:
	case 0x4116da:
	case 0x46fe43:
	case 0x45a969:
	case 0x46e432:
	case 0x46e814:
	case 0x8e5ae8:
	case 0x5d4f17:
	case 0x806531:
	case 0x5d01ae:
	case 0x7ab237:
	case 0x7c0293:
	case 0x4543a3:
	case 0x498511:
	case 0x49851c:
	case 0x8de705:
	case 0x80c931:
	case 0x7c4322:
	case 0x7c4337:
	case 0x7fdd5f:
	case 0x9963b3:
	case 0x454375:
	case 0x746b19:
	case 0x49e684:
	case 0x730077:
	case 0x8e45cd:
	case 0x72f27a:
	case 0x90dd7b:
	case 0x7ddcce:
	case 0x821e9c:
	case 0x8392f3:
	case 0x68bbc3:
	case 0x7cc0c4:
	case 0x68bbb1:
	case 0x7ae49f:
	case 0x7ae48d:
	case 0x7d050c:
	case 0x7d055d:
	case 0x6d5ac0:
	case 0x634f6f:
		return;
	}

	if (ENABLE_STR_DEBUG)
	{
		log_nl(YELLOW, "[{}] ", _ReturnAddress());

		char out[0x1000] = { 0 };

		va_list args;
		va_start(args, text);
		vsprintf_s(out, sizeof(out), text, args);
		va_end(args);

		std::string text_str(out);

		log_nl(YELLOW, text_str);

		if (ENABLE_DUMPING && _ReturnAddress() == (void*)0x40ec51 && !g_files_dump.contains(text_str))
		{
			if (text_str.ends_with('\n'))
				text_str.pop_back();

			g_files_dump.insert(text_str);
		}

		//if (text_str.find("key_kane_bikini.ee") != -1)
		//{
		//	while (!GetAsyncKeyState(VK_F3))
		//		Sleep(100);
		//}
	}
}

DEFINE_HOOK_CCALL(hash_str, 0x473720, uint32_t, std::string* str)
{
	const auto res = hash_str_hook.call(str);

	if (str && g_enable_hash_dump)
	{
		std::lock_guard lock(g_hash_dump_lock);

		if (!g_hashes.contains(*str))
			g_hashes.insert({ *str, { res, ptr(_ReturnAddress()) } });
	}

	return res;
}

DEFINE_HOOK_THISCALL(initialize_string, jc::string::fn::INIT, int, std::string* _this, const char* str, int size)
{
	// dump all strings that are not saved yet

	if (ENABLE_DUMPING)
	{
		switch (ptr(_ReturnAddress()))
		{
		case 0x486c1a:
		case 0x617ae5:
		case 0x75764d:
		case 0x70ddda:
		case 0x516131:
		case 0x487cd9:
			break;
		default:
		{
			if (g_enable_string_dump && size > 1 && str)
			{
				std::string new_str = str;

				std::lock_guard lock(g_string_dump_lock);

				if (!g_strings_dump.contains(new_str) && !new_str.starts_with("SPEED ") && !new_str.contains('\n') && !new_str.contains('\r'))
				{
					if (ENABLE_STR_DEBUG)
						log(GREEN, "New string '{}' from {}", str, _ReturnAddress());

					g_strings_dump.insert({ new_str, _ReturnAddress() });
				}
			}
		}
		}
	}

	return initialize_string_hook.call(_this, str, size);
}

DEFINE_HOOK_THISCALL(raycast, jc::physics::fn::RAYCAST, void*, uintptr_t _this, ray* r, int a1, float distance, ray_hit_info* hit_info, void* a3, bool a4, bool a5)
{
	if (_ReturnAddress() == (void*)0x646A00)
	{
		const auto ret = raycast_hook.call(_this, r, a1, distance, hit_info, a3, a4, a5);

		log(RED, "[hk_raycast] {:x} {:x}", ptr(hit_info->object), ptr(hit_info->unk));

		return ret;
	}

	return raycast_hook.call(_this, r, a1, distance, hit_info, a3, a4, a5);
}

DEFINE_HOOK_THISCALL(object_init_from_map, 0x7FA1E0, void, ObjectBase* object, object_base_map* map)
{
	if (ENABLE_INIT_FROM_MAP_DUMP)
	{
		log(WHITE, "------------------ {:x} --------------------------", ptr(object));

		map->walk();

		log(WHITE, "---------------------------------------------------");
	}
	else
	{
		log(WHITE, "------------------ {:x} --------------------------", ptr(object));

		g_record_object_map = true;

		object_init_from_map_hook.call(object, map);

		g_record_object_map = false;

		log(WHITE, "---------------------------------------------------");

		return;
	}

	object_init_from_map_hook.call(object, map);
}

DEFINE_HOOK_THISCALL(object_map_find_bool, 0x46AF70, bool, object_base_map* map, uint32_t* hash, bool* out)
{
	auto res = object_map_find_bool_hook.call(map, hash, out);

	if (res && g_record_object_map)
		log(GREEN, "map.insert<ValueType_Bool>(0x{:x}, {});", *hash, *out);

	return res;
}

DEFINE_HOOK_THISCALL(object_map_find_int16, 0x4C0030, bool, object_base_map* map, uint32_t* hash, int16_t* out)
{
	auto res = object_map_find_int16_hook.call(map, hash, out);

	if (res && g_record_object_map)
		log(GREEN, "map.insert<ValueType_Float>(0x{:x}, {:.2f}f);", *hash, jc::game::i16_to_float(*out));

	return res;
}

DEFINE_HOOK_THISCALL(object_map_find_int, 0x46AEF0, bool, object_base_map* map, uint32_t* hash, int* out)
{
	auto res = object_map_find_int_hook.call(map, hash, out);

	if (res && g_record_object_map)
		log(GREEN, "map.insert<ValueType_Int>(0x{:x}, {});", *hash, *out);

	return res;
}

DEFINE_HOOK_THISCALL(object_map_find_float, 0x46AE70, bool, object_base_map* map, uint32_t* hash, float* out)
{
	auto res = object_map_find_float_hook.call(map, hash, out);

	if (res && g_record_object_map)
		log(GREEN, "map.insert<ValueType_Float>(0x{:x}, {:.2f}f);", *hash, *out);

	return res;
}

DEFINE_HOOK_THISCALL(object_map_find_vec3, 0x50E7D0, bool, object_base_map* map, uint32_t* hash, vec3* out)
{
	auto res = object_map_find_vec3_hook.call(map, hash, out);

	if (res && g_record_object_map)
		log(GREEN, "map.insert<ValueType_Vec3>(0x{:x}, vec3 {{ {}, {}, {} }});", *hash, out->x, out->y, out->z);

	return res;
}

DEFINE_HOOK_THISCALL(object_map_find_u16vec3, 0x7A96A0, bool, object_base_map* map, uint32_t* hash, u16vec3* out)
{
	auto res = object_map_find_u16vec3_hook.call(map, hash, out);

	if (res && g_record_object_map)
		log(GREEN, "map.insert<ValueType_Vec3>(0x{:x}, vec3 {{ {}, {}, {} }});", *hash, jc::game::i16_to_float(out->x), jc::game::i16_to_float(out->y), jc::game::i16_to_float(out->z));

	return res;
}

DEFINE_HOOK_THISCALL(object_map_find_mat4, 0x4BFFB0, bool, object_base_map* map, uint32_t* hash, mat4* out)
{
	auto res = object_map_find_mat4_hook.call(map, hash, out);

	if (res && g_record_object_map)
	{
		auto m = *out;

		log(GREEN, "float mat_data[16] = {{");

		for (int i = 0; i < 16; ++i)
			log(GREEN, "{:.1f}f, ", *(float*)(ptr(out) + i * 0x4));

		log(GREEN, "}};");

		log(GREEN, "mat4 mat = *(mat4*)mat_data;");
		log(GREEN, "map.insert<ValueType_Mat4>(0x{:x}, &mat);", *hash);

		//log(GREEN, "map.insert<ValueType_Mat4>(0x{:x}, .); // put your matrix here", *hash);
	}

	return res;
}

DEFINE_HOOK_THISCALL(object_map_find_string, 0x46ADD0, bool, object_base_map* map, uint32_t* hash, jc::stl::string* out)
{
	auto res = object_map_find_string_hook.call(map, hash, out);

	if (res && g_record_object_map && out->unk > 0)
		log(*hash == 0xFBA08D60 ? RED : GREEN, "map.insert<ValueType_String>(0x{:x}, R\"({})\");", *hash, out->c_str());

	return res;
}

DEFINE_HOOK_CCALL(object_map_find_event, 0x987DD0, bool, object_base_map* map, uint32_t hash, ptr* out_ptr, jc::stl::string* out)
{
	auto res = object_map_find_event_hook.call(map, hash, out_ptr, out);

	if (res && g_record_object_map && out->length > 0)
		log(GREEN, "map.insert<ValueType_String>(0x{:x}, R\"({})\"); // weird string 1", hash, out->c_str());

	return res;
}

DEFINE_HOOK_CCALL(object_map_find_event2, 0x987E80, bool, object_base_map* map, uint32_t hash, ptr* out_ptr, jc::stl::string* out)
{
	auto res = object_map_find_event2_hook.call(map, hash, out_ptr, out);

	if (res && g_record_object_map && out->length > 0)
		log(GREEN, "map.insert<ValueType_String>(0x{:x}, R\"({})\"); // weird string 2", hash, out->c_str());

	return res;
}

DEFINE_HOOK_CCALL(object_map_find_event_and_subscribe, 0x987D20, bool, object_base_map* map, uint32_t hash, ObjectEventManager* event_manager, ptr out, jc::stl::string* name)
{
	auto res = object_map_find_event_and_subscribe_hook.call(map, hash, event_manager, out, name);

	if (res && g_record_object_map)
		log(GREEN, "map.insert<ValueType_String>(0x{:x}, R\"({})\"); // weird string 3", hash, name->c_str());

	return res;
}

DEFINE_HOOK_THISCALL(object_map_get_pointer, 0x46B050, ptr, object_base_map* map, uint32_t* hash)
{
	auto res = object_map_get_pointer_hook.call(map, hash);

	if (res && g_record_object_map)
		log(GREEN, "map.insert<ValueType_Pointer>(0x{:x}, .); // put your pointer here", *hash);

	return res;
}

void jc::clean_dbg::init()
{
	if (ENABLE_HOOKS)
	{
		// general debug

		if (ENABLE_RAYCAST_DEBUG)
			raycast_hook.hook();

		print_error_hook.hook();
		initialize_string_hook.hook();
		hash_str_hook.hook();

		// map dumper

		object_init_from_map_hook.hook();
		object_map_find_int16_hook.hook();
		object_map_find_int_hook.hook();
		object_map_find_float_hook.hook();
		object_map_find_vec3_hook.hook();
		object_map_find_u16vec3_hook.hook();
		object_map_find_mat4_hook.hook();
		object_map_find_string_hook.hook();
		object_map_find_event_hook.hook();
		object_map_find_event2_hook.hook();
		object_map_find_event_and_subscribe_hook.hook();
		object_map_get_pointer_hook.hook();

		// string dumper

		if (ENABLE_DUMPING)
		{
			const auto file_dump_path = get_solution_dir() + FILE_DUMP_PATH;

			if (!std::filesystem::is_regular_file(file_dump_path))
				std::ofstream(file_dump_path, std::ios::out);

			std::ifstream file_dump_file(file_dump_path, std::ios::in);

			std::string line;

			while (std::getline(file_dump_file, line))
				g_files_dump.insert(line);
		}

		if (ENABLE_DUMPING)
		{
			std::thread([]()
				{
					const auto string_dump_path = get_solution_dir() + STRING_DUMP_PATH;

					if (!std::filesystem::is_regular_file(string_dump_path))
						std::ofstream(string_dump_path, std::ios::out);

					std::ifstream dump_file(string_dump_path, std::ios::in);

					std::string line;

					std::regex rgx(R"((.+)\s\(created from (0x[A-Z0-9]+)\))", std::regex::optimize);

					while (std::getline(dump_file, line))
					{
						std::smatch sm;

						if (std::regex_match(line, sm, rgx))
						{
							std::lock_guard lock(g_string_dump_lock);

							g_strings_dump.insert({ sm[1].str(), (void*)std::stoi(sm[2].str(), nullptr, 16) });
						}
					}

					g_enable_string_dump = true; })
				.detach();

					std::thread([]()
						{
							const auto hash_dump_path = get_solution_dir() + HASH_DUMP_PATH;

							if (!std::filesystem::is_regular_file(hash_dump_path))
								std::ofstream(hash_dump_path, std::ios::out);

							std::ifstream dump_file(hash_dump_path, std::ios::in);

							std::string line;

							std::regex rgx(R"((.+)\s\-\>\s(0x[A-Z0-9]+)\s\(from (0x[A-Z0-9]+)\))", std::regex::optimize);

							while (std::getline(dump_file, line))
							{
								log(RED, "line {}", line);

								std::smatch sm;

								if (std::regex_match(line, sm, rgx))
								{
									std::lock_guard lock(g_hash_dump_lock);

									auto n0 = sm[1].str();
									auto n1 = sm[2].str();
									auto n2 = sm[3].str();

									n1.erase(0, 2);
									n2.erase(0, 2);

									std::stringstream ss1(n1);
									std::stringstream ss2(n2);

									uint32_t num1;
									ptr num2;

									ss1 >> std::hex >> num1;
									ss2 >> std::hex >> num2;

									g_hashes.insert({ n0, { num1, num2 } });
								}
							}

							g_enable_hash_dump = true; })
						.detach();
		}
	}
}

void jc::clean_dbg::destroy()
{
	if (ENABLE_HOOKS)
	{
		// map dumper

		object_map_get_pointer_hook.unhook();
		object_map_find_event_and_subscribe_hook.unhook();
		object_map_find_event2_hook.unhook();
		object_map_find_event_hook.unhook();
		object_map_find_string_hook.unhook();
		object_map_find_mat4_hook.unhook();
		object_map_find_u16vec3_hook.unhook();
		object_map_find_vec3_hook.unhook();
		object_map_find_float_hook.unhook();
		object_map_find_int_hook.unhook();
		object_map_find_int16_hook.unhook();
		object_init_from_map_hook.unhook();

		// general debug

		hash_str_hook.unhook();
		initialize_string_hook.unhook();
		print_error_hook.unhook();

		if (ENABLE_RAYCAST_DEBUG)
			raycast_hook.unhook();

		// string dumper

		if (ENABLE_DUMPING)
		{
			while (!g_enable_string_dump)
				SwitchToThread();

			{
				const auto string_dump_path = get_solution_dir() + STRING_DUMP_PATH;

				std::ofstream dump_file(string_dump_path, std::ios::out);

				std::lock_guard lock(g_string_dump_lock);

				for (auto [n, a] : g_strings_dump)
					dump_file << n << " (created from 0x" << std::hex << a << ")" << std::endl;
			}

			{
				const auto file_dump_path = get_solution_dir() + FILE_DUMP_PATH;

				std::ofstream file_dump_file(file_dump_path, std::ios::out);

				for (const auto& v : g_files_dump)
					file_dump_file << v << std::endl;
			}

			{
				const auto hash_dump_path = get_solution_dir() + HASH_DUMP_PATH;

				std::lock_guard lock(g_hash_dump_lock);

				std::ofstream hash_dump_file(hash_dump_path, std::ios::out);

				for (const auto& [name, hash] : g_hashes)
					hash_dump_file << name << " -> 0x" << std::uppercase << std::hex << hash.first << " (from 0x" << std::hex << hash.second << ")" << std::endl;
			}
		}
	}
}