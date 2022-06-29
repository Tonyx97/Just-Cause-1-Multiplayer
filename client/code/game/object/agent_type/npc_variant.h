#pragma once

#include "../base/base.h"

namespace jc::npc_variant
{
	// type 1 (starts at 0xC)

	static constexpr uint32_t TYPE_1_STRING_1 = 0xC;
	static constexpr uint32_t TYPE_1_STRING_2 = TYPE_1_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_1_STRING_3 = TYPE_1_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_1_STRING_4 = TYPE_1_STRING_3 + sizeof(std::string);

	// type 2 (starts at 0x7C)

	static constexpr uint32_t TYPE_2_STRING_1 = TYPE_1_STRING_4 + sizeof(std::string);
	static constexpr uint32_t TYPE_2_STRING_2 = TYPE_2_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_2_STRING_3 = TYPE_2_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_2_STRING_4 = TYPE_2_STRING_3 + sizeof(std::string);

	// 20 strings (5 different types I think) (starts at 0xEC)

	static constexpr uint32_t TYPE_3_STRING_1 = TYPE_2_STRING_4 + sizeof(std::string);
	static constexpr uint32_t TYPE_3_STRING_2 = TYPE_3_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_3_STRING_3 = TYPE_3_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_3_STRING_4 = TYPE_3_STRING_3 + sizeof(std::string);
	static constexpr uint32_t TYPE_3_STRING_5 = TYPE_3_STRING_4 + sizeof(std::string);

	static constexpr uint32_t TYPE_4_STRING_1 = TYPE_3_STRING_5 + sizeof(std::string);
	static constexpr uint32_t TYPE_4_STRING_2 = TYPE_4_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_4_STRING_3 = TYPE_4_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_4_STRING_4 = TYPE_4_STRING_3 + sizeof(std::string);
	static constexpr uint32_t TYPE_4_STRING_5 = TYPE_4_STRING_4 + sizeof(std::string);

	static constexpr uint32_t TYPE_5_STRING_1 = TYPE_4_STRING_5 + sizeof(std::string);
	static constexpr uint32_t TYPE_5_STRING_2 = TYPE_5_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_5_STRING_3 = TYPE_5_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_5_STRING_4 = TYPE_5_STRING_3 + sizeof(std::string);
	static constexpr uint32_t TYPE_5_STRING_5 = TYPE_5_STRING_4 + sizeof(std::string);

	static constexpr uint32_t TYPE_6_STRING_1 = TYPE_5_STRING_5 + sizeof(std::string);
	static constexpr uint32_t TYPE_6_STRING_2 = TYPE_6_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_6_STRING_3 = TYPE_6_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_6_STRING_4 = TYPE_6_STRING_3 + sizeof(std::string);
	static constexpr uint32_t TYPE_6_STRING_5 = TYPE_6_STRING_4 + sizeof(std::string);

	// type 7 (starts at 0x31C)

	static constexpr uint32_t TYPE_7_STRING_1 = TYPE_6_STRING_5 + sizeof(std::string);
	static constexpr uint32_t TYPE_7_STRING_2 = TYPE_7_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_7_STRING_3 = TYPE_7_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_7_STRING_4 = TYPE_7_STRING_3 + sizeof(std::string);

	// type 8 (starts at 0x38C)

	static constexpr uint32_t TYPE_8_STRING_1 = TYPE_7_STRING_4 + sizeof(std::string);
	static constexpr uint32_t TYPE_8_STRING_2 = TYPE_8_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_8_STRING_3 = TYPE_8_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_8_STRING_4 = TYPE_8_STRING_3 + sizeof(std::string);
	static constexpr uint32_t TYPE_8_STRING_5 = TYPE_8_STRING_4 + sizeof(std::string);

	// type 9 (single string) (0x418)

	static constexpr uint32_t TYPE_9_STRING = TYPE_8_STRING_5 + sizeof(std::string);

	// type 10 (starts at 0x434)

	static constexpr uint32_t TYPE_10_STRING_1 = TYPE_9_STRING + sizeof(std::string);
	static constexpr uint32_t TYPE_10_STRING_2 = TYPE_10_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_10_STRING_3 = TYPE_10_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_10_STRING_4 = TYPE_10_STRING_3 + sizeof(std::string);
	static constexpr uint32_t TYPE_10_STRING_5 = TYPE_10_STRING_4 + sizeof(std::string);

	// type 11 (starts at 0x4C0)

	static constexpr uint32_t TYPE_11_STRING_1 = TYPE_10_STRING_5 + sizeof(std::string);
	static constexpr uint32_t TYPE_11_STRING_2 = TYPE_11_STRING_1 + sizeof(std::string);
	static constexpr uint32_t TYPE_11_STRING_3 = TYPE_11_STRING_2 + sizeof(std::string);
	static constexpr uint32_t TYPE_11_STRING_4 = TYPE_11_STRING_3 + sizeof(std::string);
	static constexpr uint32_t TYPE_11_STRING_5 = TYPE_11_STRING_4 + sizeof(std::string);

	// bools (starts at 0x54C)

	static constexpr uint32_t BOOL1 = 0x54C;
	static constexpr uint32_t BOOL2 = 0x54D;
	static constexpr uint32_t BOOL3 = 0x54E;
	static constexpr uint32_t BOOL4 = 0x54F;
	static constexpr uint32_t BOOL5 = 0x550;
	static constexpr uint32_t BOOL6 = 0x551;

	static constexpr uint32_t INSTANCE_TYPE = 0x554;

	namespace fn
	{
		static constexpr uint32_t CREATE	= 0x66E5C0;
		static constexpr uint32_t ALLOCATE	= 0x671D30;
		static constexpr uint32_t SETUP		= 0x816390;
	}
}

class NPCVariant : public ObjectBase
{
public:

	static ref<NPCVariant> CREATE();
};