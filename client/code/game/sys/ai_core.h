#pragma once

namespace jc::ai_core
{
	static constexpr uint32_t SINGLETON = 0xD850E4; // AiCore*

	static constexpr uint32_t ALIVE_HANDLES = 0x600; // std::vector<some_struct_with_size_0x8>
	static constexpr uint32_t DEAD_HANDLES	= 0x610; // std::vector<CharacterHandle*>

	namespace fn
	{
		static constexpr uint32_t INSERT_CHARACTER_HANDLE = 0x584740;
	}
}

class CharacterHandle;

struct CharacterHandleEntry
{
	union
	{
		CharacterHandle* handle;

		int index;
	};

	uint32_t id;
};

class AiCore
{
public:

	void init();
	void destroy();
	void insert_character_handle(CharacterHandle* handle);
};

inline AiCore* g_ai = nullptr;