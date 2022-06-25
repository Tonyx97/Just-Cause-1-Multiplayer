#pragma once

#include <utils/utils.h>

#include <game/globals.h>
#include <game/sys/physics.h>

enum HookType
{
	Detour,
	VirtualFn
};

template <typename type, ptr address, uint32_t hash>
struct prototype
{
	using TYPE = type;

	static constexpr uint32_t HASH = hash;

	static constexpr ptr ADDRESS = address;

	static constexpr HookType hook_type = Detour;
};

template <typename type, ptr index, uint32_t hash>
struct vprototype
{
	using TYPE = type;

	static constexpr uint32_t HASH = hash;

	static constexpr ptr INDEX = index;

	static constexpr HookType hook_type = VirtualFn;
};

class Transform;
class Character;
class GameControl;
class GameStatus;

namespace jc::proto
{
	using game_present = prototype<int(__fastcall*)(void*), 0x40FB70, util::hash::JENKINS("Present")>;
	using game_tick    = prototype<void(__thiscall*)(GameControl*), 0x4F51E0, util::hash::JENKINS("Tick")>;
	using reset		   = vprototype<int(__stdcall*)(void*, void*), 16, util::hash::JENKINS("Reset")>;
	using raycast	   = prototype<void*(__thiscall*)(uintptr_t, ray*, int, float, ray_hit_info*, void*, bool, bool), jc::physics::fn::RAYCAST, util::hash::JENKINS("Raycast")>;

	namespace game_status
	{
		using dispatch = prototype<bool(__fastcall*)(GameStatus*), 0x497A70, util::hash::JENKINS("GameStatusDispatch")>;
	}

	// testing

	namespace dbg
	{
		using init_std_string = prototype<int(__thiscall*)(std::string*, const char*, int), jc::string::fn::INIT, util::hash::JENKINS("InitStdString")>;
		using print_error	  = prototype<int(__stdcall*)(const char*, ...), 0x40D0B0, util::hash::JENKINS("PrintError")>;
		using hash_str		  = prototype<uint32_t(__cdecl*)(std::string*), 0x473720, util::hash::JENKINS("HashStr")>;

		namespace map_dumper
		{
			using vcall6_obj_init = prototype<int(__thiscall*)(ptr, ptr), 0x853410, util::hash::JENKINS("vcall6_obj_init")>;
			using find_int		  = prototype<bool(__thiscall*)(ptr, uint32_t*, int*), 0x46AEF0, util::hash::JENKINS("FindInt")>;
			using find_bool		  = prototype<bool(__thiscall*)(ptr, uint32_t*, bool*), 0x46AF70, util::hash::JENKINS("FindBool")>;
			using find_int16	  = prototype<bool(__thiscall*)(ptr, uint32_t*, int16_t*), 0x4C0030, util::hash::JENKINS("FindInt16")>;
			using find_float	  = prototype<bool(__thiscall*)(ptr, uint32_t*, float*), 0x46AE70, util::hash::JENKINS("FindFloat")>;
			using find_vec3       = prototype<bool(__thiscall*)(ptr, uint32_t*, vec3*), 0x50E7D0, util::hash::JENKINS("FindVec3")>;
			using find_mat4		  = prototype<bool(__thiscall*)(ptr, uint32_t*, mat4*), 0x4BFFB0, util::hash::JENKINS("FindMat4")>;
			using find_string	  = prototype<bool(__thiscall*)(ptr, uint32_t*, std::string*), 0x46ADD0, util::hash::JENKINS("FindString")>;
			using find_string2	  = prototype<bool(__cdecl*)(ptr, uint32_t, ptr*, std::string*), 0x987DD0, util::hash::JENKINS("FindString2")>;
			using find_string3	  = prototype<bool(__cdecl*)(ptr, uint32_t, ptr*, std::string*), 0x987E80, util::hash::JENKINS("FindString3")>;
			using find_string4	  = prototype<bool(__cdecl*)(ptr, uint32_t, ptr*, ptr*, std::string*), 0x987D20, util::hash::JENKINS("FindString4")>;
		}

		using test = prototype<bool(__thiscall*)(int, int), 0x48C850, util::hash::JENKINS("test")>;
		using test2	= prototype<int(__thiscall*)(int, std::string*), 0x5C0CE0, util::hash::JENKINS("test2")>;
		// using test3 = prototype<int(__cdecl*)(int, ptr, ptr*, ptr*, std::string*), 0x987D20, utils::hash::JENKINS("test3")>;
		//  using LoadAsset = prototype<int(__thiscall*)(int, std::string*, int, const char*, int), 0x5C2DC0, utils::hash::JENKINS("test")>;
		//  using LoadModel = prototype<int(__thiscall*)(int, std::string*, Transform*, bool, bool, int), 0x57AF60, utils::hash::JENKINS("test")>;
		//  using test2 = prototype<int(__thiscall*)(int, int, Transform*, int, std::string*, int, int), 0x8ADE60, utils::hash::JENKINS("test2")>;
	}

	namespace patches
	{
		using ambience_2d_sounds		 = prototype<jc::stl::string*(__thiscall*)(int, jc::stl::string*), 0x656ED0, util::hash::JENKINS("Ambience2DSounds")>;
		using character_can_be_destroyed = prototype<bool(__fastcall*)(Character*), 0x595F10, util::hash::JENKINS("Character::CanBeDestroyed")>;
	}
}