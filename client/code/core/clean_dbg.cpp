#include <defs/standard.h>

#include "clean_dbg.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/sys/all.h>

#include <game/object/base/base.h>

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
std::atomic_bool g_enable_hash_dump = false;

std::map<std::string, std::pair<uint32_t, ptr>> g_hashes;

constexpr bool ENABLE_DUMPING = false;
constexpr bool ENABLE_STR_DEBUG = false;
constexpr bool ENABLE_MAP_DEBUG = false;

inline std::string get_solution_dir()
{
	auto solution_dir = std::string(EXPAND(SOLUTION_DIR));

	solution_dir.erase(0, 1);
	solution_dir.erase(solution_dir.size() - 2);

	return solution_dir;
}

void __stdcall hk_print_error(const char* text, ...)
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

		/**/if (text_str.find("key_kane_bikini.ee") != -1)
		{
			while (!GetAsyncKeyState(VK_F3))
				Sleep(100);
		}
	}
}

uint32_t __cdecl hk_hash_str(std::string* str)
{
	const auto res = jc::hooks::call<jc::proto::dbg::hash_str>(str);

	if (str && g_enable_hash_dump)
	{
		std::lock_guard lock(g_hash_dump_lock);

		if (!g_hashes.contains(*str))
			g_hashes.insert({ *str, { res, ptr(_ReturnAddress()) } });
	}

	return res;
}

int __fastcall hk_create_std_string(std::string* _this, void*, const char* str, int size)
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

	auto res = jc::hooks::call<jc::proto::dbg::init_std_string>(_this, str, size);

	return res;
}

void* __fastcall hk_raycast(uintptr_t _this, void*, ray* r, int a1, float distance, ray_hit_info* hit_info, void* a3, bool a4, bool a5)
{
	if (_ReturnAddress() == (void*)0x646A00)
	{
		const auto ret = jc::hooks::call<jc::proto::raycast>(_this, r, a1, distance, hit_info, a3, a4, a5);

		// log(RED, "[hk_raycast] {} {}", hit_info->object, hit_info->unk);

		return ret;
	}

	return jc::hooks::call<jc::proto::raycast>(_this, r, a1, distance, hit_info, a3, a4, a5);
}

bool g_rec_map = false;

void __fastcall hk_casp_vcall6(ptr casp, void*, ptr map)
{
	g_rec_map = ENABLE_MAP_DEBUG;

	//if (g_rec_map)
	//	log(WHITE, "------------------ {:x} --------------------------", casp);

	{
		log(WHITE, "------------------ {:x} --------------------------", casp);

		object_base_map* _map = (object_base_map*)map;

		_map->walk();

		log(WHITE, "---------------------------------------------------");
	}

	jc::hooks::call<jc::proto::dbg::map_dumper::vcall6_obj_init>(casp, map);

	//if (g_rec_map)
	//	log(WHITE, "---------------------------------------------------");

	g_rec_map = false;
}

bool __fastcall hk_map_find_bool(ptr map, void*, uint32_t* hash, bool* out)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_bool>(map, hash, out);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_Bool>(0x{:x}, {});", *hash, *out);

	return res;
}

bool __fastcall hk_map_find_int16(ptr map, void*, uint32_t* hash, int16_t* out)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_int16>(map, hash, out);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_Float>(0x{:x}, {:.2f}f);", *hash, jc::game::i16_to_float(*out));

	return res;
}

bool __fastcall hk_map_find_int(ptr map, void*, uint32_t* hash, int* out)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_int>(map, hash, out);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_Int>(0x{:x}, {});", *hash, *out);

	return res;
}

bool __fastcall hk_map_find_float(ptr map, void*, uint32_t* hash, float* out)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_float>(map, hash, out);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_Float>(0x{:x}, {:.2f}f);", *hash, *out);

	return res;
}

bool __fastcall hk_map_find_vec3(ptr map, void*, uint32_t* hash, vec3* out)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_vec3>(map, hash, out);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_Vec3>(0x{:x}, vec3 {{ {}, {}, {} }});", *hash, out->x, out->y, out->z);

	return res;
}

bool __fastcall hk_map_find_vec3i16(ptr map, void*, uint32_t* hash, u16vec3* out)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_vec3u16>(map, hash, out);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_Vec3>(0x{:x}, vec3 {{ {}, {}, {} }});", *hash, jc::game::i16_to_float(out->x), jc::game::i16_to_float(out->y), jc::game::i16_to_float(out->z));

	return res;
}

bool __fastcall hk_map_find_mat4(ptr map, void*, uint32_t* hash, mat4* out)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_mat4>(map, hash, out);

	if (res && g_rec_map)
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

bool __fastcall hk_map_find_str1(ptr map, void*, uint32_t* hash, std::string* out)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_string>(map, hash, out);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_String>(0x{:x}, R\"({})\");", *hash, *out);

	/*if (_ReturnAddress() == (void*)0x563897)
	{
		log(GREEN, "map.insert<ValueType_String>(0x{:x}, R\"({})\");", *hash, *out);

		uint32_t _hash = 0x206d0589;
		jc::stl::string out2;

		if (jc::hooks::call<jc::proto::dbg::map_dumper::find_string>(map, &_hash, (std::string*)&out2))
			log(RED, "nice! {} (size {})", out2.c_str(), ((object_base_map*)map)->size);
	}*/

	return res;
}

bool __cdecl hk_map_find_str2(ptr map, uint32_t hash, ptr* out, std::string* idk)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_string2>(map, hash, out, idk);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_String>(0x{:x}, R\"({})\"); // weird string 1", hash, *idk);

	return res;
}

bool __cdecl hk_map_find_str3(ptr map, uint32_t hash, ptr* out, std::string* idk)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_string3>(map, hash, out, idk);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_String>(0x{:x}, R\"({})\"); // weird string 2", hash, *idk);

	return res;
}

bool __cdecl hk_map_find_str4(ptr map, uint32_t hash, ptr* idk, ptr* out, std::string* idk2)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::find_string4>(map, hash, idk, out, idk2);

	if (res && g_rec_map)
		log(GREEN, "map.insert<ValueType_String>(0x{:x}, R\"({})\"); // weird string 3", hash, *idk2);

	return res;
}

mat4* __fastcall hk_map_get_mat4(ptr map, void*, uint32_t* hash)
{
	auto res = jc::hooks::call<jc::proto::dbg::map_dumper::get_mat4>(map, hash);

	if (res && g_rec_map)
	{
		auto m = *res;

		log(GREEN, "float mat_data[16] = {{");

		for (int i = 0; i < 16; ++i)
			log(GREEN, "{:.1f}f, ", *(float*)(ptr(res) + i * 0x4));

		log(GREEN, "}};");

		log(GREEN, "mat4 mat = *(mat4*)mat_data;");
		log(GREEN, "map.insert<ValueType_Mat4>(0x{:x}, &mat);", *hash);

		//log(GREEN, "map.insert<ValueType_Mat4>(0x{:x}, .); // put your matrix here", *hash);
	}

	return res;
}

void jc::clean_dbg::init()
{
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

	// general debug

	jc::hooks::hook<jc::proto::raycast>(&hk_raycast);
	jc::hooks::hook<jc::proto::dbg::print_error>(&hk_print_error);

	// map dumper

	jc::hooks::hook<jc::proto::dbg::map_dumper::vcall6_obj_init>(&hk_casp_vcall6);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_bool>(&hk_map_find_bool);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_int16>(&hk_map_find_int16);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_int>(&hk_map_find_int);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_float>(&hk_map_find_float);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_vec3>(&hk_map_find_vec3);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_vec3u16>(&hk_map_find_vec3i16);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_mat4>(&hk_map_find_mat4);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_string>(&hk_map_find_str1);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_string2>(&hk_map_find_str2);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_string3>(&hk_map_find_str3);
	jc::hooks::hook<jc::proto::dbg::map_dumper::find_string4>(&hk_map_find_str4);

	jc::hooks::hook<jc::proto::dbg::map_dumper::get_mat4>(&hk_map_get_mat4);

	// string dumper

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

		jc::hooks::hook<jc::proto::dbg::init_std_string>(&hk_create_std_string);
		jc::hooks::hook<jc::proto::dbg::hash_str>(&hk_hash_str);
	}
}

void jc::clean_dbg::destroy()
{
	// general debug

	jc::hooks::unhook<jc::proto::dbg::print_error>();
	jc::hooks::unhook<jc::proto::raycast>();

	// map dumper

	jc::hooks::unhook<jc::proto::dbg::map_dumper::vcall6_obj_init>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_bool>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_int16>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_int>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_float>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_vec3>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_vec3u16>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_string>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_string2>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_string3>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_string4>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::find_mat4>();
	jc::hooks::unhook<jc::proto::dbg::map_dumper::get_mat4>();

	// string dumper

	if (ENABLE_DUMPING)
	{
		jc::hooks::unhook<jc::proto::dbg::hash_str>();
		jc::hooks::unhook<jc::proto::dbg::init_std_string>();

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