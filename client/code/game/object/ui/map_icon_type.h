#pragma once

#include "../base/base.h"

namespace jc::ui::map_icon_type
{
}

class UIMapIconType : public ObjectBase
{
public:

	enum Hash : uint32_t
	{
		Hash_TextureName = 0x46e62875,
		Hash_NameID = 0xccd9c837,	// used across gui stuff
		Hash_ScaleX = 0x969629fc,
		Hash_ScaleY = 0x5c8f3808,
		Hash_Unk1 = 0xb0c3614f,
		Hash_Unk2 = 0x5b22b39c,
	};

	IMPL_OBJECT_TYPE_ID("CGuiMapIconType");

	bool setup(const std::string& name, const std::string& texture, const vec2& scale);
};