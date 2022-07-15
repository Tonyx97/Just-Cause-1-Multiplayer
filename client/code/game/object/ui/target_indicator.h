#pragma once

#include "../base/base.h"

namespace jc::ui::target_indicator
{
	static constexpr uint32_t COLOR = 0x6C; // u8vec4
}

class UITargetIndicator : public ObjectBase
{
public:

	IMPL_OBJECT_TYPE_ID("CTargetIndicator");

	void set_color(const u8vec4& v);

	u8vec4 get_color() const;
};