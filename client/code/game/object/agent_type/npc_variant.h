#pragma once

#include "../base/base.h"
#include "../vars/npc_variants.h"

namespace jc::npc_variant
{
	// type 1 (starts at 0xC)

	static constexpr uint32_t TYPE_1_STRING_1 = 0xC;
	static constexpr uint32_t TYPE_1_STRING_2 = TYPE_1_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_1_STRING_3 = TYPE_1_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_1_STRING_4 = TYPE_1_STRING_3 + sizeof(jc::stl::string);

	// type 2 (starts at 0x7C)

	static constexpr uint32_t TYPE_2_STRING_1 = TYPE_1_STRING_4 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_2_STRING_2 = TYPE_2_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_2_STRING_3 = TYPE_2_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_2_STRING_4 = TYPE_2_STRING_3 + sizeof(jc::stl::string);

	// 20 strings (5 different types I think) (starts at 0xEC)

	static constexpr uint32_t TYPE_3_STRING_1 = TYPE_2_STRING_4 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_3_STRING_2 = TYPE_3_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_3_STRING_3 = TYPE_3_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_3_STRING_4 = TYPE_3_STRING_3 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_3_STRING_5 = TYPE_3_STRING_4 + sizeof(jc::stl::string);

	static constexpr uint32_t TYPE_4_STRING_1 = TYPE_3_STRING_5 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_4_STRING_2 = TYPE_4_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_4_STRING_3 = TYPE_4_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_4_STRING_4 = TYPE_4_STRING_3 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_4_STRING_5 = TYPE_4_STRING_4 + sizeof(jc::stl::string);

	static constexpr uint32_t TYPE_5_STRING_1 = TYPE_4_STRING_5 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_5_STRING_2 = TYPE_5_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_5_STRING_3 = TYPE_5_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_5_STRING_4 = TYPE_5_STRING_3 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_5_STRING_5 = TYPE_5_STRING_4 + sizeof(jc::stl::string);

	static constexpr uint32_t TYPE_6_STRING_1 = TYPE_5_STRING_5 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_6_STRING_2 = TYPE_6_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_6_STRING_3 = TYPE_6_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_6_STRING_4 = TYPE_6_STRING_3 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_6_STRING_5 = TYPE_6_STRING_4 + sizeof(jc::stl::string);

	// type 7 (starts at 0x31C)

	static constexpr uint32_t TYPE_7_STRING_1 = TYPE_6_STRING_5 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_7_STRING_2 = TYPE_7_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_7_STRING_3 = TYPE_7_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_7_STRING_4 = TYPE_7_STRING_3 + sizeof(jc::stl::string);

	// type 8 (starts at 0x38C)

	static constexpr uint32_t TYPE_8_STRING_1 = TYPE_7_STRING_4 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_8_STRING_2 = TYPE_8_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_8_STRING_3 = TYPE_8_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_8_STRING_4 = TYPE_8_STRING_3 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_8_STRING_5 = TYPE_8_STRING_4 + sizeof(jc::stl::string);

	// type 9 (single string) (0x418)

	static constexpr uint32_t TYPE_9_STRING = TYPE_8_STRING_5 + sizeof(jc::stl::string);

	// type 10 (starts at 0x434)

	static constexpr uint32_t TYPE_10_STRING_1 = TYPE_9_STRING + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_10_STRING_2 = TYPE_10_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_10_STRING_3 = TYPE_10_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_10_STRING_4 = TYPE_10_STRING_3 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_10_STRING_5 = TYPE_10_STRING_4 + sizeof(jc::stl::string);

	// type 11 (starts at 0x4C0)

	static constexpr uint32_t TYPE_11_STRING_1 = TYPE_10_STRING_5 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_11_STRING_2 = TYPE_11_STRING_1 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_11_STRING_3 = TYPE_11_STRING_2 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_11_STRING_4 = TYPE_11_STRING_3 + sizeof(jc::stl::string);
	static constexpr uint32_t TYPE_11_STRING_5 = TYPE_11_STRING_4 + sizeof(jc::stl::string);

	// bools (starts at 0x54C)

	static constexpr uint32_t BOOL1 = 0x54C;
	static constexpr uint32_t BOOL2 = 0x54D;
	static constexpr uint32_t BOOL3 = 0x54E;
	static constexpr uint32_t BOOL4 = 0x54F;
	static constexpr uint32_t BOOL5 = 0x550;
	static constexpr uint32_t BOOL6 = 0x551;

	static constexpr uint32_t INSTANCE_SIZE = 0x554;

	namespace fn
	{
		static constexpr uint32_t CREATE	= 0x66E5C0;
		static constexpr uint32_t ALLOCATE	= 0x671D30;
		static constexpr uint32_t SETUP		= 0x816390;
	}
}

/**
* This class handles skins and accessories for characters, keep in mind that the engine will choose
* one of the slot pseudo-randomly if more are specified.
*/
class NPCVariant : public ObjectBase
{
public:
	enum Hash : uint32_t
	{
		// accessories 1

		Hash_Accessory1_Slot1		= 0x2630932c,
		Hash_Accessory1_Slot2		= 0xfba08d60,
		Hash_Accessory1_Slot3		= 0x4926bae9,
		Hash_Accessory1_Slot4		= 0x85b83c2f,

		// accessories 2

		Hash_Accessory2_Slot1		= 0xe7c93f5,
		Hash_Accessory2_Slot2		= 0x4845b57a,
		Hash_Accessory2_Slot3		= 0xb763e668,
		Hash_Accessory2_Slot4		= 0x635558f1,

		// accessories 3

		Hash_Accessory3_Slot1		= 0x99bf3a30,
		Hash_Accessory3_Slot2		= 0x6d55085d,
		Hash_Accessory3_Slot3		= 0x8a428310,
		Hash_Accessory3_Slot4		= 0x952d038e,

		// accessories 4

		Hash_Accessory4_Slot1		= 0x747da657,
		Hash_Accessory4_Slot2		= 0x1f27f50a,
		Hash_Accessory4_Slot3		= 0x41a90c19,
		Hash_Accessory4_Slot4		= 0x42c721b2,

		// accessories 5

		Hash_Accessory5_Slot1		= 0x3454d343,
		Hash_Accessory5_Slot2		= 0xa1ed6837,
		Hash_Accessory5_Slot3		= 0x5efe9770,
		Hash_Accessory5_Slot4		= 0xc902f8f9,

		// head skin

		Hash_HeadSkinSlot1			= 0xdb481baa,
		Hash_HeadSkinSlot2			= 0x989ff16f,
		Hash_HeadSkinSlot3			= 0x361951c5,
		Hash_HeadSkinSlot4			= 0x9100e7de,

		// cloth skin

		Hash_ClothSkinSlot1			= 0x1f56a843,
		Hash_ClothSkinSlot2			= 0xb7fc2eeb,
		Hash_ClothSkinSlot3			= 0x35ad5990,
		Hash_ClothSkinSlot4			= 0x4b69f7a8,

		Hash_ClothColor				= 0x6f190a7e,

		// attachment locations

		Hash_Accessory1_Loc			= 0x3d8bc181,
		Hash_Accessory2_Loc			= 0x6be7531e,
		Hash_Accessory3_Loc			= 0xcb2980c4,
		Hash_Accessory4_Loc			= 0x544a7cd9,
		Hash_Accessory5_Loc			= 0xfab0d659,

		// bools

		Hash_ForceAccessory1		= 0x571ca15f,
		Hash_ForceAccessory2		= 0xa9af310b,
		Hash_ForceAccessory3		= 0xdcd3fd50,
		Hash_ForceAccessory4		= 0x3bd8220c,
		Hash_ForceAccessory5		= 0xd6ecd709,
	};

	static ref<NPCVariant> CREATE();
};